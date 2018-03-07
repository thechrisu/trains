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
                 uint32_t velocity) {
  if (expected_ticks_last_sensor_hit == NO_LAST_SENSOR) {
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
    int distance_diff = (int32_t)velocity * ticks_diff / 10000;
    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dHTime diff: %c%d.%d%d s - Distance diff: %c%d.%d cm%s",
                  TRAIN_LOCATION_LINE + 2, 1,
                  ticks_diff < 0 ? '-' : ' ',
                  ABS(ticks_diff) / 100, ABS(ticks_diff / 10) % 10, ABS(ticks_diff) % 10,
                  distance_diff < 0 ? '-' : ' ',
                  ABS(distance_diff) / 10, ABS(distance_diff) % 10,
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

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHSensor Prediction Spot%s:%s",
                TRAIN_LOCATION_LINE, 1, TRADEMARK, HIDE_CURSOR_TO_EOL) == 0);

  train_data tr_data;
  reply_get_last_sensor_hit seen_sensor;
  turnout_state turnout_states[NUM_TURNOUTS];

  unsigned int last_sensor;
  unsigned int expected_last_sensor = NO_LAST_SENSOR;
  int ticks_last_sensor_hit;
  int expected_ticks_last_sensor_hit;
  int expected_ticks_next_sensor_hit = NO_LAST_SENSOR;
  int loops = 0;

  get_last_sensor_hit(sensor_interpreter_tid, t1train, &seen_sensor);
  last_sensor = seen_sensor.sensor;
  ticks_last_sensor_hit = seen_sensor.ticks;

  while (true) {
    get_train(track_state_controller_tid, t1train, &tr_data);
    get_last_sensor_hit(sensor_interpreter_tid, t1train, &seen_sensor);

    if (last_sensor != seen_sensor.sensor && seen_sensor.sensor != NO_DATA_RECEIVED) {
      last_sensor = seen_sensor.sensor;
      ticks_last_sensor_hit = seen_sensor.ticks;
      expected_ticks_last_sensor_hit = expected_ticks_next_sensor_hit;

      get_turnouts(track_state_controller_tid, turnout_states);
      unsigned int next_sensor = sensor_next(&track, last_sensor, turnout_states);

      message reply;
      get_constant_velocity_model(track_state_controller_tid, t1train, &reply);
      int velocity = reply.msg.train_speeds[tr_data.should_speed];

      if (next_sensor != NO_NEXT_SENSOR && velocity != 0) {
        int distance_to_next_sensor = distance_between_sensors(&track, last_sensor, next_sensor);
        int ticks_to_next_sensor = distance_to_next_sensor * 10000 / velocity;
        expected_ticks_next_sensor_hit = ticks_last_sensor_hit + ticks_to_next_sensor;
      }

      print_next_sensor_prediction(terminal_tx_server_tid,
                                   next_sensor,
                                   expected_ticks_next_sensor_hit);
      print_diffs(terminal_tx_server_tid,
                  expected_last_sensor, last_sensor,
                  expected_ticks_last_sensor_hit, ticks_last_sensor_hit,
                  velocity);

      expected_last_sensor = next_sensor;
    }

    loops += 1;
    DelayUntil(clock_server_tid, REFRESH_PERIOD * loops);
  }
}
