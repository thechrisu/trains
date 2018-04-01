#include "train_conductor.h"

#define CONDUCTOR_STOP_CHECK_INTERVAL 2
#define CONDUCTOR_SENSOR_CHECK_INTERVAL 1

#define SWITCH_LOOKAHEAD_NODES 10

#define SWITCH_DIST 25000

#define ABS(a) ((a) > 0 ? (a) : (-(a)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

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
  int dist = 100 * distance_between_track_nodes_helper(find_sensor(&track, start_sensor),
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

/**
 * Given a triggered notification, perform some actions and send new
 * requests for notifications.
 *
 * @param clock_server                Clock server tid.
 * @param train_tx_server             Train tx server tid.
 * @param n                           Notification we received.
 * @param route                       Route we're on (May change).
 * @param end                         Goal of our route (necessary for rerouting).
 * @param drop_existing_notifications Output: Should all other notifications
 *                                    be dropped?
 * @param got_lost                    Output: Whether we lost the train.
 * @return Whether we should stop the train.
 */
bool process_location_notification(int clock_server, int train_tx_server,
                                   location_notification *n,
                                   track_node *route[MAX_ROUTE_LENGTH],
                                   location *end,
                                   bool *drop_existing_notifications,
                                   bool *got_lost) {
  *got_lost = false;
  switch (n->reason) {
    case GOT_LOST: // OH SNAP! Wait until we're localized again.
      *drop_existing_notifications = true;
      *got_lost = true;
      return false;
      break;
    case LOCATION_CHANGED: {
#if DEBUG_TRAIN_COORDINATOR
      logprintf("changed locations to %s +- %d\n\r", n->subject.loc.node->name, n->subject.loc.offset);
#endif /* DEBUG_TRAIN_COORDINATOR */
      *drop_existing_notifications = true;
      if (on_route(route, &n->subject.loc)) {
        return false;
      }
      return reroute(&n->subject.loc, end, route);
    }
    case LOCATION_TO_SWITCH:
#if DEBUG_TRAIN_COORDINATOR
      logprintf("switching %d to %s\n\r", n->action.switch_to_switch[0], n->action.switch_to_switch[1] ? "C" : "S");
#endif /* DEBUG_TRAIN_COORDINATOR */
      switcher_turnout(clock_server,
                       train_tx_server,
                       n->action.switch_to_switch[0],
                       n->action.switch_to_switch[1]);
      *drop_existing_notifications = false;
      return false;
    case LOCATION_TO_STOP: {
      *drop_existing_notifications = true;
#if DEBUG_TRAIN_COORDINATOR
      logprintf("STAHP\n\r");
#endif /* DEBUG_TRAIN_COORDINATOR */
      return true;
    }
    case LOCATION_ANY:
#if DEBUG_TRAIN_COORDINATOR
      logprintf("Got LOCATION_ANY to %s +- %d\n\r", n->subject.loc.node->name, n->subject.loc.offset);
#endif /* DEBUG_TRAIN_COORDINATOR */
      *drop_existing_notifications = true;
      return reroute(&n->subject.loc, end, route);
    default:
      logprintf("Unexpected notification type %d\n\r", n->reason);
      Assert(0 && "Unexpected notification type");
      return true;
  }
}

void craft_new_triggers(coordinates *c, uint32_t train_speeds[15],
                        uint32_t train_distances[15],
                        track_node *route[MAX_ROUTE_LENGTH],
                        bool got_lost,
                        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
                        int *n_requests) {
  *n_requests = 0;
  if (got_lost) {
    return;
  }
  bool has_next_turnout;
  coordinates f;
  tmemcpy(&f, c, sizeof(f));
  do {
    has_next_turnout = false;
    bool next_switch_is_curved;
    int next_turnout_num;
    coordinates where_to_switch;
    predict_next_switch(&f, route, &where_to_switch, &next_turnout_num,
        &next_switch_is_curved, &has_next_turnout, SWITCH_DIST, SWITCH_LOOKAHEAD_NODES);
    if (has_next_turnout) {
      f.loc.node = turnout_num_to_node(&track, next_turnout_num);
      f.loc.offset = 0;
      tmemcpy(&locations_to_observe[*n_requests].subject.loc, &where_to_switch, sizeof(where_to_switch));
      locations_to_observe[*n_requests].reason = LOCATION_TO_SWITCH;
      locations_to_observe[*n_requests].action.switch_to_switch[0] = next_turnout_num;
      locations_to_observe[*n_requests].action.switch_to_switch[1] = next_switch_is_curved;
      *n_requests = *n_requests + 1;
      /*logprintf("Send switch here: %s +- %d (switch: %d, pos; %s)\n\r",
          where_to_switch.loc.node->name, where_to_switch.loc.offset,
          next_turnout_num, next_switch_is_curved ? "curved" : "straight");*/
    }
    f.loc.offset++;
  } while (has_next_turnout && *n_requests < 5);
  coordinates target;
  predict_train_stop(c, route, &target, train_speeds, train_distances);
  tmemcpy(&locations_to_observe[*n_requests].subject.loc, &target.loc, sizeof(target.loc));
  locations_to_observe[*n_requests].reason = LOCATION_TO_STOP;
  *n_requests = *n_requests + 1;
}

void set_new_triggers(int coord_courier,
                      coordinates *c, track_node *route[MAX_ROUTE_LENGTH],
                      uint32_t train_speeds[15], uint32_t train_distances[15],
                      bool drop_existing_notifications, bool got_lost) {
  message next_req;
  next_req.type = REPLY_CONDUCTOR_NOTIFY_REQUEST;
  next_req.msg.notification_request.drop_existing = drop_existing_notifications;
  craft_new_triggers(c, train_speeds,
                     train_distances,
                     route, got_lost,
                     next_req.msg.notification_request.notifications,
                     &next_req.msg.notification_request.num_requests);
  Assert(Reply(coord_courier, &next_req, sizeof(next_req)) == 0);
}

void route_to_within_stopping_distance(int clock_server, int train_tx_server,
                                       int track_state_controller, int train_coordinates_server,
                                       int train, int sensor_offset, int goal_offset) {
  location end = { .node = find_sensor(&track, sensor_offset), .offset = goal_offset };

  message velocity_model;
  get_constant_velocity_model(track_state_controller, train,
                              &velocity_model);

  message stopping_distance_model;
  get_stopping_distance_model(track_state_controller, train,
                              &stopping_distance_model);

  track_node *route[MAX_ROUTE_LENGTH];

  bool had_to_reverse = false;

  coordinates c;
  get_coordinates(train_coordinates_server, train, &c);

  if (c.loc.node == NULL_TRACK_NODE && c.current_speed == 0) {
    conductor_setspeed(train_tx_server, track_state_controller, train, 12);
  }

  while (c.loc.node == NULL_TRACK_NODE) {
    Delay(clock_server, CONDUCTOR_SENSOR_CHECK_INTERVAL);
    get_coordinates(train_coordinates_server, train, &c);
  }

  if (c.loc.node == end.node && c.loc.offset > end.offset) {
    conductor_reverse_to_speed(train_tx_server, track_state_controller,
                               clock_server, train, 0);
    get_coordinates(train_coordinates_server, train, &c);
    had_to_reverse = true;

#if ROUTING_DEBUG
    logprintf("Had to reverse!\n\r");
#endif /* ROUTING_DEBUG */
  }

#if ROUTING_DEBUG
  logprintf("We are at: %s\n\r", c.loc.node->name);
#endif /* ROUTING_DEBUG */

  int route_result = get_route(&c.loc, &end, route);
  if (route_result < 0) {
    conductor_reverse_to_speed(train_tx_server, track_state_controller,
                               clock_server, train, 0);
    get_coordinates(train_coordinates_server, train, &c);

    route_result = get_route(&c.loc, &end, route);
    if (route_result < 0) {
      logprintf("Tried to route from %s to %s but couldn't get a route\n\r",
                c.loc.node->name, end.node->name);
      return;
    }
  }

  int dist_left = ABS(get_remaining_dist_in_route(route, &c.loc));
  int max_feasible_speed = get_max_feasible_speed(
                              dist_left,
                              stopping_distance_model.msg.train_distances);

#if ROUTING_DEBUG
  logprintf("Max feasible speed: %d, left: %d\n\r", max_feasible_speed, dist_left);
  logprintf("current: %d, acc: %d\n\r", c.current_speed, c.acceleration);
#endif /* ROUTING_DEBUG */

  if (max_feasible_speed != -1 && (had_to_reverse || c.current_speed == 0)) {
#if ROUTING_DEBUG
    logprintf("Setting speed to %d\n\r", max_feasible_speed);
#endif /* ROUTING_DEBUG */

    conductor_setspeed(train_tx_server, track_state_controller,
                       train, max_feasible_speed);
  }

  if (max_feasible_speed == -1) {
    return;
  }

  int coord_courier = create_courier(train);

  int s = Time(clock_server);
  bool should_quit = false;
  bool received_kill = false;

  while (!should_quit) {
    int sender_tid;
    message received;

    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
    Assert(sender_tid == coord_courier);

    Assert(Time(clock_server) - s <= 50 * 100);

    bool got_lost = false;
    bool drop_existing_notifications = false;

    switch (received.type) {
      case MESSAGE_SUNSET:
        should_quit = true;
        received_kill = true;
        break;
      case MESSAGE_CONDUCTOR_NOTIFY_REQUEST:
        // logprintf("Got message of type %d\n\r", received.msg.notification_response.reason);
        should_quit = process_location_notification(
                                          clock_server, train_tx_server,
                                          &received.msg.notification_response,
                                          route, &end,
                                          &drop_existing_notifications,
                                          &got_lost);
        if (!should_quit) {
          get_coordinates(train_coordinates_server, train, &c);
          set_new_triggers(coord_courier, &c, route,
                           velocity_model.msg.train_speeds,
                           stopping_distance_model.msg.train_distances,
                           drop_existing_notifications, got_lost);
        }
        break;
      default:
        logprintf("%s%d%s%d\n\r",
            "Unexpected message type in route to sd in conductor for train ",
            train, " got message type: ", received.type);
        Assert(0 && "Unexpected message type in route withing stopping d");
        break;
    }
  }

  Assert(Kill(coord_courier) == 0);
  if (received_kill) Exit();
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
                        int track_state_controller, int train_coordinates_server,
                        int train, int sensor_offset, int goal_offset) {
  route_to_within_stopping_distance(clock_server, train_tx_server,
                                    track_state_controller, train_coordinates_server,
                                    train, sensor_offset, goal_offset);
  train_data tr_data;
  get_train(track_state_controller, train, &tr_data);
  conductor_setspeed(train_tx_server, track_state_controller, train, 0);

  coordinates c;
  get_coordinates(train_coordinates_server, train, &c);
  location end = { .node = find_sensor(&track, sensor_offset), .offset = goal_offset };

  track_node *route[MAX_ROUTE_LENGTH];
  get_route(&c.loc, &end, route);

  int n_switched = num_turnouts_within_distance(track_state_controller, route, &c.loc, 200000);
  switch_turnouts_within_distance(clock_server, train_tx_server, track_state_controller,
                                  route, &c.loc, 200000);

  int total_delay = 30 * tr_data.should_speed - n_switched * 15;
  if (tr_data.should_speed == 0 || total_delay <= 0) return;
  Assert(Delay(clock_server, total_delay) == 0);
}

void conductor_loop(int clock_server, int train_tx_server,
                    int track_state_controller, int train_coordinates_server,
                    int train, int speed) {
  unsigned int D5 = sensor_offset('D', 5);

  conductor_setspeed(train_tx_server, track_state_controller, train, speed);
  route_to_within_stopping_distance(clock_server, train_tx_server,
                                    track_state_controller, train_coordinates_server,
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

  // Important to get these turnouts switched ASAP
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
  int train_coordinates_server = WhoIs("TrainCoordinatesServer");
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);
  Assert(cmd_dispatcher > 0);
  Assert(train_coordinates_server > 0);

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
      case MESSAGE_SUNSET:
        Exit(); // Don't want to send a READY message
        break;
      case MESSAGE_USER:
        switch (received.msg.cmd.type) {
          case USER_CMD_SD:
            conductor_calib_sd(train_tx_server, track_state_controller,
                               clock_server, terminal_tx_server,
                               d.train, received.msg.cmd.data[1]);
            break;
          case USER_CMD_LOOP:
            conductor_loop(clock_server, train_tx_server,
                           track_state_controller, train_coordinates_server,
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
                            track_state_controller, train_coordinates_server,
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
