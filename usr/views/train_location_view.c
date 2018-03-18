#include "train_location_view.h"

#define NO_LAST_SENSOR 0x1EADBEEF

#define ABS(x) ((x) < 0 ? -(x) : (x))

void print_next_sensor_prediction(int terminal_tx_server_tid,
                                  unsigned int next_sensor,
                                  int expected_ticks_next_sensor_hit) {
  if (next_sensor == NO_NEXT_SENSOR) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHAt end of track%s",
                  TRAIN_LOCATION_LINE + 1, 1, HIDE_CURSOR_TO_EOL) == 0);
  } else if (expected_ticks_next_sensor_hit == NO_LAST_SENSOR) {
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
  if (expected_ticks_last_sensor_hit == NO_LAST_SENSOR) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHNo diffs available%s",
                  TRAIN_LOCATION_LINE + 2, 1, HIDE_CURSOR_TO_EOL) == 0);
  } else if (expected_last_sensor != last_sensor && expected_last_sensor != NO_NEXT_SENSOR) {
    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dHExpected to hit %c%d but hit %c%d instead%s",
                  TRAIN_LOCATION_LINE + 2, 1,
                  sensor_bank(expected_last_sensor), sensor_index(expected_last_sensor),
                  sensor_bank(last_sensor), sensor_index(last_sensor),
                  HIDE_CURSOR_TO_EOL) == 0);
  } else if (expected_last_sensor == NO_NEXT_SENSOR) {
    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dH%s",
                  TRAIN_LOCATION_LINE + 2, 1, HIDE_CURSOR_TO_EOL) == 0);
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

  coordinates current_prediction, last_prediction;
  reply_get_last_sensor_hit last_sensor;

  turnout_state turnout_states[NUM_TURNOUTS];

  int loops = 0;

  while (true) {
    reply_get_last_sensor_hit seen_sensor;
    get_last_sensor_hit(sensor_interpreter_tid, t1train, &seen_sensor);

    if (seen_sensor.sensor != last_sensor.sensor) {
      tmemcpy(&last_sensor, &seen_sensor, sizeof(last_sensor));

      print_diffs(terminal_tx_server_tid,
                  current_prediction.loc.sensor, last_sensor.sensor,
                  current_prediction.ticks, last_sensor.ticks,
                  distance_diff(last_sensor.sensor, &current_prediction.loc));
    }

    if (seen_sensor.sensor != last_sensor.sensor || loops % 5 == 0) {
      tmemcpy(&last_prediction, &current_prediction, sizeof(last_prediction));

      get_turnouts(track_state_controller_tid, turnout_states);
      predict_sensor_hit(train_coordinates_server_tid,
                         turnout_states,
                         t1train, &current_prediction);

      if (current_prediction.loc.sensor != last_prediction.loc.sensor ||
          current_prediction.ticks / 10 != last_prediction.ticks / 10) {
        print_next_sensor_prediction(terminal_tx_server_tid,
                                     current_prediction.loc.sensor,
                                     current_prediction.ticks);
      }
    }

    loops += 1;
    DelayUntil(clock_server_tid, REFRESH_PERIOD * loops);
  }
}
