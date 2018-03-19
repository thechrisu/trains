#include "sensor_interpreter.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))

#define NO_FIRST_SENSOR_EXPECTED 1234
#define NOT_ATTRIBUTED           1337

static unsigned int last_sensor[80];
static int time_at_last_sensor_hit[80];
static int terminal_tx_server;

void attribute_sensor(int train, unsigned int sensor, int current_time) {
  last_sensor[train] = sensor;
  time_at_last_sensor_hit[train] = current_time;

  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%c%d%s",
                ESC, CALIB_LINE, 1,
                "Train ", train, " just hit sensor ", sensor_bank(sensor), sensor_index(sensor),
                HIDE_CURSOR_TO_EOL) == 0);
}

/**
 * If the train is moving at speed 1, give it 100 seconds to find itself.
 * Otherwise, give it 42/`speed` seconds (e.g. 21 for speed 2, 3 for speed 14).
 *
 * @param   speed           The current speed of the train.
 * @param   time_difference The time since the last sensor read attributed to the train.
 * @returns Whether or not to give the train more time to cross an attributable sensor.
 **/
bool train_is_lost(int speed, int time_difference) {
  return speed == 0 || time_difference > (speed == 1 ? 100 * 100 : 42 * 100 / speed);
}

void sensor_interpreter() {
  int sender_tid;
  message received, reply;

  for (int i = 0; i < 80; i += 1) {
    last_sensor[i] = NO_DATA_RECEIVED;
    time_at_last_sensor_hit[i] = 0;
  }

  int16_t current_sensors[10], leading_edge[10];
  for (int i = 0; i < 10; i += 1) {
    current_sensors[i] = 0;
  }

  unsigned int expected_first_sensors[81];
  for (int i = 0; i < 81; i += 1) {
    expected_first_sensors[i] = NO_FIRST_SENSOR_EXPECTED;
  }
  expected_first_sensors[24] = sensor_offset('A', 5);
  expected_first_sensors[74] = sensor_offset('A', 8);

  int clock_server_tid = WhoIs("ClockServer");
  terminal_tx_server = WhoIs("TerminalTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  int train_coordinates_server = WhoIs("TrainCoordinatesServer");
  Assert(train_coordinates_server > 0);

  RegisterAs("SensorInterpreter");

  while (1) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));

    switch (received.type) {
      case MESSAGE_SENSORSRECEIVED: {
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

        get_leading_edge(current_sensors, received.msg.sensors, leading_edge);

        bool no_new_sensor_hits = true;
        for (int i = 0; i < 10; i += 1) {
          if (leading_edge[i] != 0) {
            no_new_sensor_hits = false;
            break;
          }
        }

        if (no_new_sensor_hits) {
          break;
        }

        int current_time = Time(clock_server_tid);

        train_data tr_data[81];
        for (int i = 0; i < num_active_trains; i += 1) {
          int train = active_trains[i];
          get_train(track_state_controller_tid, train, &tr_data[train]);
        }

        turnout_state turnouts[NUM_TURNOUTS];
        get_turnouts(track_state_controller_tid, turnouts);

        int sensor_attributed_to = NOT_ATTRIBUTED;

        for (unsigned int sensor = 0; sensor < 80; sensor += 1) {
          if (is_sensor_triggered(leading_edge, sensor)) {
            for (int i = 0; i < num_active_trains; i += 1) {
              int train = active_trains[i];
              unsigned int last = last_sensor[train];

              if ((last == NO_DATA_RECEIVED &&
                   sensor == expected_first_sensors[train]) ||
                  (last != NO_DATA_RECEIVED &&
                   sensor == sensor_next(&track, last, turnouts))) {
                attribute_sensor(train, sensor, current_time);
                sensor_attributed_to = train;
                break;
              }
            }

            if (sensor_attributed_to == NOT_ATTRIBUTED) {
              for (int i = 0; i < num_active_trains; i += 1) {
                int train = active_trains[i];
                train_data *data = &tr_data[train];
                unsigned int last = last_sensor[train];
                int last_time = time_at_last_sensor_hit[train];

                if (last != NO_DATA_RECEIVED &&
                    sensor_may_be_seen_next(&track, last, sensor)) {
                  if (sensor_is_followed_by(&track, last, sensor) &&
                      last_time - data->time_speed_last_changed >
                      40 * ABS(data->should_speed - data->last_speed)) {
                      int time_elapsed = current_time - last_time;

                      update_constant_velocity_model(track_state_controller_tid,
                                                     train, data->should_speed,
                                                     last, sensor,
                                                     time_elapsed);

                      Assert(Printf(terminal_tx_server,
                                    "%s%d;%dH%s%d%s%d%s%c%d%s%c%d%s%d%s",
                                    ESC, CALIB_LINE + 1, 1,
                                    "Train ", train,
                                    " took ", time_elapsed,
                                    " ticks to go between sensors ",
                                    sensor_bank(last), sensor_index(last),
                                    " and ",
                                    sensor_bank(sensor), sensor_index(sensor),
                                    " at speed ", data->should_speed,
                                    HIDE_CURSOR_TO_EOL) == 0);
                  }
                }

                attribute_sensor(train, sensor, current_time);
                sensor_attributed_to = train;
                break;
              }
            }
          }

          if (sensor_attributed_to == NOT_ATTRIBUTED) {
            for (int i = 0; i < num_active_trains; i += 1) {
              int train = active_trains[i];
              int last_time = time_at_last_sensor_hit[train];

              if (train_is_lost(tr_data[train].should_speed, current_time - last_time)) {
                attribute_sensor(train, sensor, current_time);
                sensor_attributed_to = train;
                break;
              }
            }
          }
        }

        if (sensor_attributed_to != NOT_ATTRIBUTED) {
          message send;
          send.type = MESSAGE_UPDATE_COORDS_SENSOR;
          send.msg.update_coords.tr_data.train = sensor_attributed_to;
          send.msg.update_coords.last_sensor.sensor = last_sensor[sensor_attributed_to];
          send.msg.update_coords.last_sensor.ticks = time_at_last_sensor_hit[sensor_attributed_to];
          Assert(Send(train_coordinates_server,
                      &send, sizeof(send),
                      EMPTY_MESSAGE, 0) == 0);
        }

        break;
      }
      case MESSAGE_GET_LAST_SENSOR_HIT: {
        int train = received.msg.train;
        Assert(train > 0 && train <= 80);

        reply.type = REPLY_GET_LAST_SENSOR_HIT;
        reply.msg.last_sensor.sensor = last_sensor[train];
        reply.msg.last_sensor.ticks = time_at_last_sensor_hit[train];

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      default:
        Assert(0);
        break;
    }

  }
}
