#include "train_location_view.h"

#define NO_LAST_SENSOR 0x1EADBEEF

#define ABS(x) ((x) < 0 ? -(x) : (x))

void train_location_view() {
  message send, reply;

  int clock_server_tid = WhoIs("ClockServer");
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = WhoIs("TrackStateController");
  Assert(track_state_controller_tid > 0);

  int terminal_tx_server_tid = WhoIs("TerminalTxServer");
  Assert(terminal_tx_server_tid > 0);

  int sensor_interpreter_tid = WhoIs("SensorInterpreter");
  Assert(sensor_interpreter_tid > 0);

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHSensor Prediction Spot:%s%s",
                TRAIN_LOCATION_LINE, 1, TRADEMARK, HIDE_CURSOR_TO_EOL) == 0);

  train_data tr_data;
  unsigned int last_sensor;
  unsigned int expected_last_sensor;
  int time_last_sensor_hit;
  int expected_time_last_sensor_hit;
  int expected_time_next_sensor_hit = NO_LAST_SENSOR;
  int loops = 0;

  get_last_sensor_hit(sensor_interpreter_tid, t1train, &reply);
  last_sensor = reply.msg.last_sensor.sensor;
  time_last_sensor_hit = reply.msg.last_sensor.time;

  while (true) {
    send.type = MESSAGE_GETTRAIN;
    send.msg.tr_data.train = t1train;
    Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
    Assert(reply.type == REPLY_GETTRAIN);
    tmemcpy(&tr_data, &reply.msg.tr_data, sizeof(tr_data));

    get_last_sensor_hit(sensor_interpreter_tid, t1train, &reply);
    unsigned int seen_sensor = reply.msg.last_sensor.sensor;

    if (last_sensor != seen_sensor && seen_sensor != NO_DATA_RECEIVED) {
      last_sensor = seen_sensor;
      time_last_sensor_hit = reply.msg.last_sensor.time;

      send.type = MESSAGE_GETTURNOUTS;
      Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
      Assert(reply.type == REPLY_GETTURNOUTS);

      unsigned int next_sensor = sensor_next(&track, last_sensor, reply.msg.turnout_states);

      if (next_sensor == 1337) {
        Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHAt end of track%s",
                      TRAIN_LOCATION_LINE + 1, 1, HIDE_CURSOR_TO_EOL) == 0);
      } else {
        expected_time_last_sensor_hit = expected_time_next_sensor_hit;

        send.type = MESSAGE_GETCONSTANTSPEEDMODEL;
        send.msg.train = t1train;
        Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
        Assert(reply.type == REPLY_GETCONSTANTSPEEDMODEL);

        int velocity = reply.msg.train_speeds[tr_data.should_speed];
        int time_to_next_sensor = distance_between_sensors(&track, last_sensor, next_sensor) * 10000 / velocity;
        expected_time_next_sensor_hit = time_last_sensor_hit + time_to_next_sensor;

        uint32_t minutes = expected_time_next_sensor_hit / 100 / 60;
        uint32_t seconds = (expected_time_next_sensor_hit / 100) % 60;
        uint32_t deciseconds = (expected_time_next_sensor_hit / 10) % 10;

        Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHNext sensor: %s%c%d - Expected time: %d:%s%d.%d%s",
                      TRAIN_LOCATION_LINE + 1, 1,
                      sensor_index(next_sensor) >= 10 ? "" : " ",
                      sensor_bank(next_sensor), sensor_index(next_sensor),
                      minutes, seconds >= 10 ? "" : "0", seconds, deciseconds,
                      HIDE_CURSOR_TO_EOL) == 0);

        if (expected_last_sensor == last_sensor && expected_time_last_sensor_hit != NO_LAST_SENSOR) {
          int time_diff = time_last_sensor_hit - expected_time_last_sensor_hit;
          int distance_diff = velocity * time_diff / 10000;
          Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHTime diff: %c%u.%u%u s - Distance diff: %c%u.%u cm%s",
                        TRAIN_LOCATION_LINE + 2, 1,
                        time_diff < 0 ? '-' : ' ',
                        ABS(time_diff) / 100, ABS(time_diff / 10) % 10, ABS(time_diff) % 10,
                        distance_diff < 0 ? '-' : ' ',
                        ABS(distance_diff) / 10, ABS(distance_diff) % 10,
                        HIDE_CURSOR_TO_EOL) == 0);
        } else {
          Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHNo diffs available%s",
                        TRAIN_LOCATION_LINE + 2, 1, HIDE_CURSOR_TO_EOL) == 0);
        }
      }

      expected_last_sensor = next_sensor;
    }

    loops += 1;
    DelayUntil(clock_server_tid, REFRESH_PERIOD * loops);
  }
}
