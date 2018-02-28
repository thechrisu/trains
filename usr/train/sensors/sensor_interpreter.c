#include "sensor_interpreter.h"

#define NO_DATA_RECEIVED 0xDEDEDEDE

#define ABS(x) ((x) > 0 ? (x) : -(x))

void sensor_interpreter() {
  int sender_tid;
  message received;

  unsigned int last_sensor[80];
  int time_at_last_sensor_hit[80];
  for (int i = 0; i < 80; i += 1) {
    last_sensor[i] = NO_DATA_RECEIVED;
    time_at_last_sensor_hit[i] = 0;
  }

  int16_t current_sensors[10], leading_edge[10];
  for (int i = 0; i < 10; i += 1) {
    current_sensors[i] = 0;
  }

  int clock_server_tid = WhoIs("ClockServer");
  int terminal_tx_server = WhoIs("TerminalTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  RegisterAs("SensorInterpreter");

  while (1) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));

    switch (received.type) {
      case MESSAGE_SENSORSRECEIVED: {
        get_leading_edge(current_sensors, received.msg.sensors, leading_edge);

        int current_time = Time(clock_server_tid);

        for (unsigned int sensor = 0; sensor < 80; sensor += 1) {
          if (is_sensor_triggered(leading_edge, sensor)) {
            unsigned int last = last_sensor[t1train];

            if (last == NO_DATA_RECEIVED || sensors_are_paired(&track, last, sensor)) {
              last_sensor[t1train] = sensor;
              time_at_last_sensor_hit[t1train] = current_time;
            } else if (sensor_is_followed_by(&track, last, sensor)) {
              message send, reply;
              send.type = MESSAGE_GETTRAIN;
              send.msg.tr_data.train = t1train;
              Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

              int current_speed = reply.msg.tr_data.should_speed;
              int last_speed = reply.msg.tr_data.last_speed;
              int last_time = time_at_last_sensor_hit[t1train];
              int time_speed_last_changed = reply.msg.tr_data.time_speed_last_changed;

              if (last_time - time_speed_last_changed > 40 * ABS(current_speed - last_speed)) {
                int time_elapsed = current_time - last_time;

                update_constant_velocity_model(track_state_controller_tid, t1train, current_speed, last, sensor, time_elapsed);

                char start_bank = sensor_bank(last);
                unsigned int start_index = sensor_index(last);
                char end_bank = sensor_bank(sensor);
                unsigned int end_index = sensor_index(sensor);

                Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s%c%d%s%c%d%s%d%s",
                              ESC, CALIB_LINE, 1,
                              "Train ", t1train, " took ", time_elapsed, " ticks to go between sensors ",
                              start_bank, start_index, " and ", end_bank, end_index, " at speed ", current_speed,
                              HIDE_CURSOR_TO_EOL) == 0);

              }

              last_sensor[t1train] = sensor;
              time_at_last_sensor_hit[t1train] = current_time;
            }
          }
        }
        break;
      }
      default:
        Assert(0);
        break;
    }

    Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
  }
}
