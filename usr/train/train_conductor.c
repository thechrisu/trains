#include "train_conductor.h"

#define CONDUCTOR_STOP_CHECK_INTERVAL 2
#define CONDUCTOR_SENSOR_CHECK_INTERVAL 1

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
  return (int)velocity * (c_time - ticks_at_last_sensor) / 100;
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
    if (terminal_tx_server > 0)
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
  while ((c + 1)->train != 0) {
    if (c->node->type == NODE_BRANCH) {
      if ((c + 1)->node == STRAIGHT(c->node)) {
        switch_turnout(clock_server, train_tx_server, track_state_controller, c->node->num, false);
      } else if ((c + 1)->node == CURVED(c->node)) {
        switch_turnout(clock_server, train_tx_server, track_state_controller, c->node->num, true);
      } else {
        logprintf("Route has switch nodes %d -> %d, but they're not connected\n\r",
                  c->node->num, (c + 1)->node->num);
      }
    }
    c += 1;
  }
}

void get_location_from_last_sensor_hit(int clock_server, int velocity,
                                       reply_get_last_sensor_hit *last_record,
                                       location *current) {
  current->sensor = last_record->sensor;
  current->offset = velocity * (Time(clock_server) - last_record->ticks) / 100;
}

int get_remaining_dist_in_route(reservation *remaining_route) {
  int dist_remaining_100th_mm = 0;
  reservation *c = remaining_route;
  while ((c + 1)->train != 0) {
    switch (c->node->type) {
      case NODE_SENSOR:
      case NODE_MERGE:
      case NODE_ENTER:
        dist_remaining_100th_mm += c->node->edge[DIR_AHEAD].dist * 100;
        break;
      case NODE_BRANCH:
        if ((c + 1)->node == STRAIGHT(c->node)) {
          dist_remaining_100th_mm += c->node->edge[DIR_STRAIGHT].dist * 100;
        } else {
          dist_remaining_100th_mm += c->node->edge[DIR_CURVED].dist * 100;
        }
        break;
      default:
        logprintf("Invalid node type when getting remaining distance of route: %d\n\r", c->node->type);
        break;
    }
    c += 1;
  }
  return dist_remaining_100th_mm;
}

// TODO comment
reservation *get_next_of_type(reservation *remaining_route, node_type type) {
  logprintf("get_next_of_type: remaining route starts at %s\n\r", remaining_route->node->name);
  logprintf("get_next_of_type: type to look for is %d\n\r", type);
  reservation *c = remaining_route + 1;
  while (c->train != 0) {
    logprintf("get_next_of_type: looking at %s of type %d\n\r", c->node->name, c->node->type);
    if (c->node->type == type) {
      logprintf("get_next_of_type: returning %s\n\r", c->node->name);
      return c;
    }
  }
  logprintf("get_next_of_type: returning NULL_RESERVATION\n\r");
  return NULL_RESERVATION;
}

int get_dist_between_reservations(reservation *start, reservation *end) {
  int dist_remaining_100th_mm = 0;
  reservation *c = start;
  while ((c + 1)->train != 0 || c != end) {
    switch (c->node->type) {
      case NODE_SENSOR:
      case NODE_MERGE:
      case NODE_ENTER:
        dist_remaining_100th_mm += c->node->edge[DIR_AHEAD].dist * 100;
        break;
      case NODE_BRANCH:
        if ((c + 1)->node == STRAIGHT(c->node)) {
          dist_remaining_100th_mm += c->node->edge[DIR_STRAIGHT].dist * 100;
        } else {
          dist_remaining_100th_mm += c->node->edge[DIR_CURVED].dist * 100;
        }
        break;
      default:
        logprintf("Invalid node type when getting distance between nodes: %d\n\r", c->node->type);
        break;
    }
    c += 1;
  }
  return dist_remaining_100th_mm;
}

void route_to_within_stopping_distance(int clock_server, int train_tx_server,
                                       int track_state_controller, int sensor_interpreter,
                                       int train, int sensor_offset, int goal_offset) {
  int s = Time(clock_server);
  location start, end;
  reply_get_last_sensor_hit last_record;

  message velocity_model, stopping_distance_model;
  get_constant_velocity_model(track_state_controller, train,
                              &velocity_model);
  get_stopping_distance_model(track_state_controller, train,
                              &stopping_distance_model);

  train_data tr_data;
  get_train(track_state_controller, train, &tr_data);
  int speed = tr_data.should_speed == 0 ? tr_data.last_speed : tr_data.should_speed;

  end.sensor = sensor_offset;
  end.offset = goal_offset;
  reservation route[MAX_ROUTE_LENGTH];
  bool got_error = false;

  get_last_sensor_hit(sensor_interpreter, train, &last_record);
  while (last_record.sensor == NO_DATA_RECEIVED) {
    Delay(clock_server, CONDUCTOR_SENSOR_CHECK_INTERVAL);
    get_last_sensor_hit(sensor_interpreter, train, &last_record);
  }

  do {
    got_error = false;
    get_location_from_last_sensor_hit(clock_server,
            (int)velocity_model.msg.train_speeds[speed], &last_record, &start);

    int route_result = get_route_next(train, speed, &start, &end, route);
    if (route_result < 0) {
      logprintf("Tried to route from %c%d to %c%d but couldn't get a route\n\r",
                sensor_bank(start.sensor), sensor_index(start.sensor),
                sensor_bank(end.sensor), sensor_index(end.sensor));
      return;
    }

    logprintf("Got route from %c%d to %c%d with %d nodes\n\r",
        sensor_bank(start.sensor), sensor_index(start.sensor),
        sensor_bank(end.sensor), sensor_index(end.sensor),
        route_length(route));
    for (int i = 0; i < route_length(route); i += 1) {
      logprintf("%s\n\r", route[i].node->name);
    }

    reservation *c = (reservation *)route;
    route_switch_turnouts(clock_server, train_tx_server, track_state_controller,
                          route); // TODO maybe do this via switchers?
    bool is_first_sensor = true;
    while (c->train != 0) {
      if (Time(clock_server) - s > 100 * 50) Assert(0);

      int dist_left = get_remaining_dist_in_route(c) -
                      dist_from_last_sensor(clock_server, last_record.ticks,
                                            velocity_model.msg.train_speeds[speed]);
      int stopping_distance = (int)stopping_distance_model.msg.train_distances[speed];

      // logprintf("Last sensor: %c%d, dist left: %d, stopping distance: %d\n\r", sensor_bank(last_record.sensor), sensor_index(last_record.sensor), dist_left, stopping_distance);

      if (dist_left < stopping_distance) {
        break; // TODO add delay to only return once we have stopped??
      } else {
        reply_get_last_sensor_hit sensor_hit_polling_result;
        get_last_sensor_hit(sensor_interpreter, train, &sensor_hit_polling_result);

        if (sensor_hit_polling_result.sensor != last_record.sensor) {
          logprintf("New sensor data: %c%d\n\r",
              sensor_bank(sensor_hit_polling_result.sensor),
              sensor_index(sensor_hit_polling_result.sensor));
          last_record.sensor = sensor_hit_polling_result.sensor;
          last_record.ticks = sensor_hit_polling_result.ticks;

          reservation *next_sensor = get_next_of_type(c, NODE_SENSOR);
          logprintf("next_sensor is null: %s\n\r", next_sensor == NULL_RESERVATION ? "yes" : "no");
          if (next_sensor != NULL_RESERVATION) {
            if (last_record.sensor == (unsigned int)next_sensor->node->num || is_first_sensor) {
              logprintf("Updated c to %s\n\r", next_sensor->node->name);
              c = next_sensor;
              is_first_sensor = false;
            } else {
              logprintf("Expected to be at sensor %s but were at %c%d\n\r",
                  next_sensor->node->name,
                  sensor_bank(last_record.sensor), sensor_index(last_record.sensor));
              got_error = true; // Oh no! We are lost and we should reroute.
              break;
            }
          }
        }
      }

      Delay(clock_server, 1);
    }

    cancel_route(train);

    Assert(!got_error);
  } while (got_error);
}

/**
 * Routes a train to a sensor.
 *
 * @param clock_server                 Tid of the clock server.
 * @param train_tx_server              Transmit server of the train.
 * @param track_state_controller       Track state controller.
 * @param train                        Train number to route.
 * @param sensor_offset                Offset of sensor (1-80)
 * @param goal_offset                  Offset from goal sensor (in 1/100mm)
 */
void conductor_route_to(int clock_server, int train_tx_server,
                        int track_state_controller, int sensor_interpreter,
                        int train, int sensor_offset, int goal_offset) {
  route_to_within_stopping_distance(clock_server, train_tx_server,
                                    track_state_controller, sensor_interpreter,
                                    train, sensor_offset, goal_offset);
  conductor_setspeed(train_tx_server, track_state_controller, train, 0);
}

void conductor_loop(int clock_server, int train_tx_server,
                    int track_state_controller, int sensor_interpreter,
                    int train, int speed) {
  unsigned int D5 = sensor_offset('D', 5);

  conductor_setspeed(train_tx_server, track_state_controller, train, speed);
  route_to_within_stopping_distance(clock_server, train_tx_server,
                                    track_state_controller, sensor_interpreter,
                                    train, D5, 0);

  bool timed_out = poll_until_sensor_pair_triggered_with_timeout(
    clock_server,
    track_state_controller,
    D5,
    60 * 100
  );

  if (timed_out) {
    conductor_setspeed(train_tx_server, track_state_controller, train, 0);
    return;
  }

  // Important to get these sensors switched ASAP
  switch_turnout(clock_server, train_tx_server, track_state_controller, 8, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 10, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 17, false);

  switch_turnout(clock_server, train_tx_server, track_state_controller, 9, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 13, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 14, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 15, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 16, false);
}

void train_conductor() {
  int sender_tid;
  message received, ready;
  int train_tx_server = WhoIs("TrainTxServer");
  int terminal_tx_server = WhoIs("TerminalTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int cmd_dispatcher = WhoIs("CommandDispatcher");
  int sensor_interpreter = WhoIs("SensorInterpreter");
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);
  Assert(cmd_dispatcher > 0);

  train_data d;
  d.last_speed = 0;
  d.should_speed = 0;
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
          case USER_CMD_LOOP:
            conductor_loop(clock_server, train_tx_server,
                           track_state_controller, sensor_interpreter,
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
            conductor_route_to(clock_server, train_tx_server,
                            track_state_controller, sensor_interpreter,
                            d.train, received.msg.cmd.data[1], received.msg.cmd.data[2] * 100);
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
