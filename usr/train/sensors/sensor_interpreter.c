#include "sensor_interpreter.h"

#define ABS(x) ((x) > 0 ? (x) : -(x))

#define TICKS_TO_CHANGE_ONE_SPEED 40

#define NOT_ATTRIBUTED            1337

static unsigned int last_sensor[80];
static int time_at_last_sensor_hit[80];
static int terminal_tx_server;

void attribute_sensor(int train, unsigned int sensor, int current_time) {
  last_sensor[train] = sensor;
  time_at_last_sensor_hit[train] = current_time;

  if (sensor >= 80) {
    logprintf("Attributing sensor >= 80: %d\n\r", sensor);
  }
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%c%d%s",
                ESC, CALIB_LINE, 1,
                "Train ", train, " just hit sensor ", sensor_bank(sensor), sensor_index(sensor),
                HIDE_CURSOR_TO_EOL) == 0);
}

/**
 * If the train is stopped, assume it isn't lost.
 * If the train is moving at speed 1, give it 100 seconds to find itself.
 * Otherwise, give it 42/`speed` seconds (e.g. 21 for speed 2, 3 for speed 14).
 *
 * @param   speed           The current speed of the train.
 * @param   time_difference The time since the last sensor read attributed to the train.
 * @returns Whether or not to give the train more time to cross an attributable sensor.
 **/
bool train_is_lost(int speed, int time_difference) {
  return speed != 0 && time_difference > (speed == 1 ? 100 * 100 : 42 * 100 / speed);
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

        if (!any_sensor_is_triggered(leading_edge)) {
          break;
        }

        int current_time = Time(clock_server_tid);

        int trains_to_attribute_to[80];
        int num_trains_to_attribute_to;

        if (num_groups == 1) {
          train_group *g = &tr_groups[0].g;
          for (int i = 0; i < g->num_members; i += 1) {
            trains_to_attribute_to[i] = g->members[i];
          }
          for (int i = 0; i < num_active_trains; i += 1) {
            trains_to_attribute_to[g->num_members + i] = active_trains[i];
          }
          num_trains_to_attribute_to = g->num_members + num_active_trains;
        } else {
          tmemcpy(trains_to_attribute_to, active_trains, num_active_trains * sizeof(int));
          num_trains_to_attribute_to = num_active_trains;
        }

        train_data tr_data[81];
        for (int i = 0; i < num_trains_to_attribute_to; i += 1) {
          int train = trains_to_attribute_to[i];
          get_train(track_state_controller_tid, train, &tr_data[train]);
        }

        turnout_state turnouts[NUM_TURNOUTS];
        get_turnouts(track_state_controller_tid, turnouts);

        int sensor_attributed_to = NOT_ATTRIBUTED;

        for (unsigned int sensor = 0; sensor < 80; sensor += 1) {
          if (is_sensor_triggered(leading_edge, sensor)) {
            for (int i = 0; i < num_trains_to_attribute_to; i += 1) {
              int train = trains_to_attribute_to[i];
              unsigned int last = last_sensor[train];

              if (last == NO_DATA_RECEIVED &&
                  sensor == expected_next_sensors[train]) {
                attribute_sensor(train, sensor, current_time);
                sensor_attributed_to = train;
                break;
              }
            }

            if (sensor_attributed_to == NOT_ATTRIBUTED) {
              for (int i = 0; i < num_trains_to_attribute_to; i += 1) {
                int train = trains_to_attribute_to[i];
                train_data *data = &tr_data[train];
                unsigned int last = last_sensor[train];
                int last_time = time_at_last_sensor_hit[train];

                if (last != NO_DATA_RECEIVED &&
                    sensor_is_followed_by(&track, last, sensor)) {
                  int time_diff = last_time - data->time_speed_last_changed;
                  int ticks_to_change_speed = TICKS_TO_CHANGE_ONE_SPEED *
                                              ABS(data->should_speed - data->last_speed);

                  if (time_diff > ticks_to_change_speed) {
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

                  attribute_sensor(train, sensor, current_time);
                  sensor_attributed_to = train;
                  break;
                }
              }
            }

            if (sensor_attributed_to == NOT_ATTRIBUTED) {
              for (int i = 0; i < num_trains_to_attribute_to; i += 1) {
                int train = trains_to_attribute_to[i];
                unsigned int last = last_sensor[train];

                if (last != NO_DATA_RECEIVED &&
                    sensor_may_be_seen_next(&track, last, sensor)) {
                  attribute_sensor(train, sensor, current_time);
                  sensor_attributed_to = train;
                  break;
                }
              }
            }

            if (sensor_attributed_to == NOT_ATTRIBUTED) {
              for (int i = 0; i < num_trains_to_attribute_to; i += 1) {
                int train = trains_to_attribute_to[i];
                int last_time = time_at_last_sensor_hit[train];

                if (train_is_lost(tr_data[train].should_speed, current_time - last_time)) {
                  attribute_sensor(train, sensor, current_time);
                  sensor_attributed_to = train;
                  break;
                }
              }
            }
          }
        }

        if (sensor_attributed_to != NOT_ATTRIBUTED) {
          send_attributed_sensor_to_coord_server(
                  train_coordinates_server, sensor_attributed_to,
                  last_sensor[sensor_attributed_to],
                  time_at_last_sensor_hit[sensor_attributed_to]);
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

void send_attributed_sensor_to_coord_server(int coord_server, int train,
                                            unsigned int ls, unsigned int tls) {
  message send;
  send.type = MESSAGE_UPDATE_COORDS_SENSOR;
  send.msg.update_coords.tr_data.train = train;
  send.msg.update_coords.last_sensor.sensor = ls;
  send.msg.update_coords.last_sensor.ticks = tls;
  Assert(Send(coord_server,
              &send, sizeof(send),
              EMPTY_MESSAGE, 0) == 0);
}
