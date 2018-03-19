#include "train_location_view.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

void print_next_sensor_prediction(int terminal_tx_server_tid,
                                  unsigned int next_sensor,
                                  int expected_ticks_next_sensor_hit) {
  if (next_sensor == NO_NEXT_SENSOR) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHAt end of track%s",
                  TRAIN_LOCATION_LINE + 1, 1, HIDE_CURSOR_TO_EOL) == 0);
  } else if (expected_ticks_next_sensor_hit == INFINITE_TICKS) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHNext sensor: %s%c%d%s",
                  TRAIN_LOCATION_LINE + 1, 1,
                  sensor_index(next_sensor) >= 10 ? "" : " ",
                  sensor_bank(next_sensor), sensor_index(next_sensor),
                  HIDE_CURSOR_TO_EOL) == 0);
  } else {
    uint32_t minutes = expected_ticks_next_sensor_hit / 100 / 60;
    uint32_t seconds = (expected_ticks_next_sensor_hit / 100) % 60;
    uint32_t deciseconds = (expected_ticks_next_sensor_hit / 10) % 10;

    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dHNext sensor: %s%c%d - Expected time: %d:%s%d.%d%s",
                  TRAIN_LOCATION_LINE + 1, 1,
                  sensor_index(next_sensor) >= 10 ? "" : " ",
                  sensor_bank(next_sensor), sensor_index(next_sensor),
                  minutes, seconds >= 10 ? "" : "0", seconds, deciseconds,
                  HIDE_CURSOR_TO_EOL) == 0);
  }
}

void print_diffs(int terminal_tx_server_tid,
                 unsigned int expected_last_sensor, unsigned int last_sensor,
                 int expected_ticks_last_sensor_hit, int ticks_last_sensor_hit,
                 int distance_diff) {
  if (expected_last_sensor == NO_NEXT_SENSOR) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHNo diffs available%s",
                  TRAIN_LOCATION_LINE + 2, 1, HIDE_CURSOR_TO_EOL) == 0);
  } else if (expected_last_sensor != last_sensor) {
    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dHExpected to hit %c%d but hit %c%d instead%s",
                  TRAIN_LOCATION_LINE + 2, 1,
                  sensor_bank(expected_last_sensor), sensor_index(expected_last_sensor),
                  sensor_bank(last_sensor), sensor_index(last_sensor),
                  HIDE_CURSOR_TO_EOL) == 0);
  } else {
    int ticks_diff = ticks_last_sensor_hit - expected_ticks_last_sensor_hit;
    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dHTime diff: %c%d.%d%d s - Distance diff: %c%d.%d cm%s",
                  TRAIN_LOCATION_LINE + 2, 1,
                  ticks_diff < 0 ? '-' : ' ',
                  ABS(ticks_diff) / 100, ABS(ticks_diff / 10) % 10, ABS(ticks_diff) % 10,
                  distance_diff < 0 ? '-' : ' ',
                  ABS(distance_diff / 100) / 10, ABS(distance_diff / 100) % 10,
                  HIDE_CURSOR_TO_EOL) == 0);
  }
}

void train_location_view() {
  int clock_server_tid = WhoIs("ClockServer");
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = WhoIs("TrackStateController");
  Assert(track_state_controller_tid > 0);

  int terminal_tx_server_tid = WhoIs("TerminalTxServer");
  Assert(terminal_tx_server_tid > 0);

  int sensor_interpreter_tid = WhoIs("SensorInterpreter");
  Assert(sensor_interpreter_tid > 0);

  int train_coordinates_server_tid = WhoIs("TrainCoordinatesServer");
  Assert(train_coordinates_server_tid > 0);

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHSensor Prediction Spot%s:%s",
                TRAIN_LOCATION_LINE, 1, TRADEMARK, HIDE_CURSOR_TO_EOL) == 0);

  turnout_state turnout_states[NUM_TURNOUTS];
  get_turnouts(track_state_controller_tid, turnout_states);

  coordinates current_prediction;
  predict_sensor_hit(train_coordinates_server_tid,
                     turnout_states,
                     active_trains[0], &current_prediction);

  coordinates current;
  get_coordinates(train_coordinates_server_tid, active_trains[0], &current);

  reply_get_last_sensor_hit last_sensor;
  get_last_sensor_hit(sensor_interpreter_tid, active_trains[0], &last_sensor);

  int loops = 0;

  while (true) {
    reply_get_last_sensor_hit seen_sensor;
    get_last_sensor_hit(sensor_interpreter_tid, active_trains[0], &seen_sensor);

    if (seen_sensor.sensor != last_sensor.sensor) {
      get_turnouts(track_state_controller_tid, turnout_states);

      print_diffs(terminal_tx_server_tid,
                  current_prediction.loc.sensor, seen_sensor.sensor,
                  current_prediction.ticks, seen_sensor.ticks,
                  distance_diff(&track, turnout_states,
                                seen_sensor.sensor, &current.loc));

      predict_sensor_hit(train_coordinates_server_tid,
                         turnout_states,
                         active_trains[0], &current_prediction);

      print_next_sensor_prediction(terminal_tx_server_tid,
                                   current_prediction.loc.sensor,
                                   current_prediction.ticks);
    } else if (seen_sensor.sensor != NO_DATA_RECEIVED) {
      get_turnouts(track_state_controller_tid, turnout_states);
      coordinates tentative_prediction;
      predict_sensor_hit(train_coordinates_server_tid,
                         turnout_states,
                         active_trains[0], &tentative_prediction);

      if (tentative_prediction.loc.sensor == current_prediction.loc.sensor) {
        if (tentative_prediction.ticks / 10 != current_prediction.ticks / 10) {
          print_next_sensor_prediction(terminal_tx_server_tid,
                                       tentative_prediction.loc.sensor,
                                       tentative_prediction.ticks);
        }

        tmemcpy(&current_prediction, &tentative_prediction, sizeof(current_prediction));
      } else {
        print_next_sensor_prediction(terminal_tx_server_tid,
                                     tentative_prediction.loc.sensor,
                                     tentative_prediction.ticks);
      }
    }

    tmemcpy(&last_sensor, &seen_sensor, sizeof(last_sensor));

    get_coordinates(train_coordinates_server_tid, active_trains[0], &current);

    loops += 1;
    DelayUntil(clock_server_tid, REFRESH_PERIOD * loops);
  }
}
