#include "train_conductor.h"

#define CONDUCTOR_STOP_CHECK_INTERVAL 5

void conductor_setspeed(int train_tx_server, int track_state_controller,
                        int train, int speed) {
  set_train_speed(train_tx_server, track_state_controller,
                  train, speed);
}

void conductor_reverse_to_speed(int train_tx_server,
                                int track_state_controller, int clock_server,
                                int train, int speed) {
  stop_and_reverse_train_to_speed(clock_server, train_tx_server,
                                  track_state_controller, train, speed);
}

/**
 * Returns the distance we travelled since we last hit a sensor.
 *
 * @param clock_server           Tid of the clock server.
 * @param ticks_at_last_sensor   The time in ticks at the point when we last hit a sensor.
 * @param velocity               The train's velocity (in 1/100th mm/s).
 *
 * @return The distance in 1/100th mm that we have travelled.
 */
int dist_from_last_sensor(int clock_server, int ticks_at_last_sensor,
                          uint32_t velocity) {
  int c_time = Time(clock_server);
  return velocity * (c_time - ticks_at_last_sensor) / 100;
}

/**
 * Blocks until the train has travelled dist.
 *
 * @param clock_server             Tid of the clock server.
 * @param terminal_tx_server       Tid of the terminal transmit server.
 * @param dist                     Distance to travel ('1' is 1/100thm).
 * @param velocity                 Velocity of the train (1/100thmm/s).
 */
void poll_until_at_dist(int clock_server, int terminal_tx_server,
                  int dist, int velocity) {
  int last_stopping = Time(clock_server);
  int start_dist = dist;
  while (dist > 0) {
    Delay(clock_server, CONDUCTOR_STOP_CHECK_INTERVAL);
    int diff = dist_from_last_sensor(clock_server, last_stopping, velocity);
    dist = start_dist - diff;
    Assert(Printf(terminal_tx_server, "%s%d;%dH(%d left - step %d)%s", ESC,
      CALIB_LINE + 2, 1, dist, diff, HIDE_CURSOR_TO_EOL) == 0);
  }
}

// Only performs one iteration of calibration
void conductor_calib_sd_one_iter(int train_tx_server, int track_state_controller,
                                 int clock_server, int terminal_tx_server,
                                 int train, int speed) {
  message velocity_model, stopping_distance_model, stopping_time_model;
  get_constant_velocity_model(track_state_controller, train,
                              &velocity_model);
  get_stopping_distance_model(track_state_controller, train,
                              &stopping_distance_model);
  get_stopping_time_model(track_state_controller, train,
                          &stopping_time_model);
  set_train_speed_and_headlights(train_tx_server, track_state_controller,
                                 train, 0, true);
  unsigned int start_sensor, goal_sensor, reverse_start_sensor;
  if (speed == 14) {
    start_sensor = sensor_offset('D', 9);
    goal_sensor = sensor_offset('B', 10);
  } else if (speed <= 9) {
    start_sensor = sensor_offset('D', 11);
    goal_sensor = sensor_offset('C', 16);
  } else if (speed >= 12) {
    start_sensor = sensor_offset('E', 12);
    goal_sensor = sensor_offset('B', 10);
  } else {
    start_sensor = sensor_offset('C', 8);
    goal_sensor = sensor_offset('B', 10);
  }
  reverse_start_sensor = sensor_pair(&track, start_sensor);
  int dist = 100 * distance_between_sensors_helper(find_sensor(&track, start_sensor),
                                                   find_sensor(&track, goal_sensor),
                                                   0, 15);
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s%d%s", ESC, CALIB_LINE + 2,
    1, "Calibrating stopping distance for train ", train, " and speed ", speed,
    " distance between sensors is: ", dist, HIDE_CURSOR_TO_EOL) == 0);
  conductor_reverse_to_speed(train_tx_server, track_state_controller,
                             clock_server, train, 10);
  poll_until_sensor_triggered(clock_server, track_state_controller,
                              reverse_start_sensor);
  Delay(clock_server, 30 * speed);
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s%s", ESC, CALIB_LINE + 2,
    1,"Calibrating stopping distance for train ", train, " and speed ", speed,
    " now back to sensor...", HIDE_CURSOR_TO_EOL) == 0);
  conductor_reverse_to_speed(train_tx_server, track_state_controller,
                             clock_server, train, speed);
  poll_until_sensor_triggered(clock_server, track_state_controller,
                              start_sensor);
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s%d%s", ESC,
    CALIB_LINE + 2, 1,
    "Calibrating stopping distance for train ", train, " and speed ", speed,
    " now waiting until we are close ...", velocity_model.msg.train_speeds[speed],
    HIDE_CURSOR_TO_EOL) == 0);
  poll_until_at_dist(clock_server, terminal_tx_server,
               dist - stopping_distance_model.msg.train_distances[speed],
               velocity_model.msg.train_speeds[speed]);
  int stop_start = Time(clock_server);
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s%s", ESC, CALIB_LINE + 2,
    1, "Calibrating stopping distance for train ", train, " and speed ", speed,
    " now stopping...", HIDE_CURSOR_TO_EOL) == 0);
  conductor_setspeed(train_tx_server, track_state_controller, train, 0);
  bool did_hit_sensor = !poll_until_sensor_triggered_with_timeout(
                                               clock_server,
                                               track_state_controller,
                                               goal_sensor,
                                               speed * 50);
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s%s%d", ESC, CALIB_LINE + 2,
    1, "Calibrating stopping distance for train ", train, " and speed ", speed,
    " stopped - dit hit sensor: ", did_hit_sensor ? 1 : 0, HIDE_CURSOR_TO_EOL) == 0);
  int ticks_after_poll = Time(clock_server);
  if (did_hit_sensor) {
    int ticks_to_stop = ticks_after_poll - stop_start;
    // logprintf("Hit sensor, off by %d\n\r", distance_off);
    stopping_distance_model.msg.train_distances[speed]
      = (uint32_t)((int)stopping_distance_model.msg.train_distances[speed] + 2500);
    update_stopping_distance_model(track_state_controller, train, speed,
                                   stopping_distance_model.msg.train_distances[speed]);
    update_stopping_time_model(track_state_controller, train, speed,
                               ticks_to_stop * 10000);
  } else {
    conductor_setspeed(train_tx_server, track_state_controller, train, 1);
    poll_until_sensor_triggered(clock_server, track_state_controller, goal_sensor);
    conductor_setspeed(train_tx_server, track_state_controller, train, 0);
    int distance_off = ((int)velocity_model.msg.train_speeds[1])
                         * (Time(clock_server) - ticks_after_poll) / 100;
    // logprintf("%d, did not hit sensor\n\r", distance_off);
    Assert(Printf(terminal_tx_server, "%s%d;%dHAdjusting stopping distance by %d%s",
                  ESC, CALIB_LINE + 2, 1, distance_off, HIDE_CURSOR_TO_EOL) == 0);
    update_stopping_distance_model(track_state_controller, train, speed,
                                   (uint32_t)((int)stopping_distance_model.msg.train_distances[speed] - distance_off));
  }
  conductor_setspeed(train_tx_server, track_state_controller, train, 0);
}

void conductor_calib_sd(int train_tx_server, int track_state_controller,
                        int clock_server, int terminal_tx_server,
                        int train, int speed) {
  switch_turnout(clock_server, train_tx_server, track_state_controller, 3, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 5, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 6, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 7, speed >= 10);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 8, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 9, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 11, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 15, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 18, false);

  for (int i = 0; i < 3; i++) {
    conductor_calib_sd_one_iter(train_tx_server, track_state_controller,
                                clock_server, terminal_tx_server, train, speed);
  }
}

void route_switch_turnouts(int clock_server, int train_tx_server,
                           int track_state_controller, reservation *route) {
  reservation *c = route;
  while (c->train != 0) {
    if (c->node->type == NODE_BRANCH) {
      if ((c + 1)->node->train != 0) {
        if (c->node->edge[DIR_STRAIGHT] == (c + 1)->node) {
          switch_turnout(clock_server, train_tx_server, track_state_controller, c->node->num, false);
        } else if (c->node->edge[DIR_CURVED] == (c + 1)->node) {
          switch_turnout(clock_server, train_tx_server, track_state_controller, c->node->num, true);
        } else {
          logprintf("Route has switch nodes %d -> %d, but they're not connected\n\r",
                    c->node->num, (c + 1)->node->num);
        }
      }
    }
    c += 1;
  }
}

void get_next_two_sensors(reservation *remaining_route, int next_two_sensors[2]) {
  int n_sensors_found;
  reservation *c = remaining_route
  next_two_sensors[0] = -1; next_two_sensors[1] = -1;
  while (n_sensors_found < 2 && c->train != 0) {
    if (c->node->type == NODE_SENSOR) {
      next_two_sensors[n_sensors_found] = c->node->num;
      n_sensors_found += 1;
    }
    c += 1;
  }
}

/**
 * Routes a train to a sensor.
 *
 * @param clock_server                 Tid of the clock server.
 * @param train_tx_server              Transmit server of the train.
 * @param track_state_controller       Track state controller.
 * @param train                        Train number to route.
 * @param speed                        Train speed to route.
 * @param sensor_offset                Offset of sensor (1-80)
 * @param goal_offset                  Offset from goal sensor (in 1/100mm)
 */
void conductor_route_to(int clock_server, int train_tx_server,
                        int track_state_controller, int train, int speed,
                        int sensor_offset, int goal_offset) {
  location current, end;
  message sensor_message;
  int current_speed = speed;
  start.sensor = -1;
  start.offset = -1;
  end.sensor = sensor_offset;
  end.offset = goal_offset;
  reservation route[MAX_ROUTE_LENGTH];
  bool got_error = false;
  do { got_error = false; Assert(get_route(train, speed, &start, &end, route) == 0); // TODO fix for T2
    reservation *c = (reservation *)route;
    route_switch_turnouts(clock_server, train_tx_server, track_state_controller,
                          route); // TODO maybe do this via switchers?
    conductor_setspeed(train_tx_server, track_state_controller, train, current_speed);
    while (c->train != 0) {
      get_sensors(track_state_controller, &sensor_message);
      int next_two_sensors[2];
      get_next_two_sensors(c, next_two_sensors);
      if (next_two_sensors[0] == -1); // TODO no sensor left -> you're on your own, buddy.
      else if (next_two_sensors[1] == -1); // TODO if this sensor fails, you're also on your own.
      else ; // We're comfortable and can endure at least one sensor failure
      switch (c->node->type) {
        case NODE_SENSOR:
          // TODO poll with timeout until at that sensor OR the sensor after that.
          break;
        case NODE_BRANCH:
          // TODO take into account broken turnouts
        default:
          logprintf("Unknown route node type %d in conductor_route_to_sensor\n\r",
                    c->node->type);
      }
    }
  } while (got_error);
}

void train_conductor() {
  int sender_tid;
  message received, ready;
  int train_tx_server = WhoIs("TrainTxServer");
  int terminal_tx_server = WhoIs("TerminalTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int cmd_dispatcher = WhoIs("CommandDispatcher");
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);
  Assert(cmd_dispatcher > 0);

  train_data d;
  d.last_speed = 0;
  d.time_speed_last_changed = Time(clock_server);

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
  Assert(received.msg.train > 0 && received.msg.train <= 80);
  Assert(received.type == MESSAGE_CONDUCTOR_SETTRAIN);
  d.train = received.msg.train;

  ready.msg.train = d.train;
  ready.type = MESSAGE_READY;
  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
    switch (received.type) {
      case MESSAGE_USER:
        switch (received.msg.cmd.type) {
          case USER_CMD_SD:
            conductor_calib_sd(train_tx_server, track_state_controller,
                               clock_server, terminal_tx_server,
                               d.train, received.msg.cmd.data[1]);
            break;
          case USER_CMD_TR:
            Assert(received.msg.cmd.data[0] == d.train);
            conductor_setspeed(train_tx_server, track_state_controller, d.train,
                               received.msg.cmd.data[1]);
            break;
          case USER_CMD_RV:
            Assert(received.msg.cmd.data[0] == d.train);
            stop_and_reverse_train(clock_server, train_tx_server,
                                   track_state_controller, d.train);
            break;
          case USER_CMD_R:
            Assert(received.msg.cmd.data[0] == d.train);
            conductor_route_to_sensor(clock_server, train_tx_server,
                            track_state_controller, d.train, d.should_speed,
                            received.msg.cmd.data[1], received.msg.cmd.data[2] * 100);
            break;
          default:
            logprintf("Got user cmd message of type %d\n\r", received.msg.cmd.type);
            Assert(0);
            break;
        }
        Assert(Send(cmd_dispatcher, &ready, sizeof(ready), EMPTY_MESSAGE, 0) == 0);
        break;
      default:
        logprintf("Got user cmd message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
