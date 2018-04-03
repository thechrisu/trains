#include "coordinate_courier.h"

#define TOO_MANY_NOTIFICATION_REQUESTS -1
#define ABS(a) ((a) < 0 ? -(a) : (a))

/**
 * Helper function, useful to diagnose problems arising from a starved notification.
 * (So far, we haven't had any problems tho)
 *
 * @Param locations_to_observe             Possible notification slows.
 * @param is_location_set                  Map to valid notification slots.
 * @param c                                Our location + speed.
 */
void print_num_triggerable_notifications(
          location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
          bool is_location_set[MAX_LOCATIONS_TO_OBSERVE],
          coordinates *c) {
  int n_notifications = 0;
  for (int i = 0; i < MAX_LOCATIONS_TO_OBSERVE; i++) {
    if (is_location_set[i]
        && location_is_ge(&c->loc, &locations_to_observe[i].subject.loc)) {
      n_notifications += 1;
    }
  }
  if (n_notifications > 1) {
    logprintf("Notifications: %d\n\r", n_notifications);
  }
}

bool coordinates_to_notification(coordinates *c, coordinates *last,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE],
        location_notification *n) {
  if (c->loc.node == NULL_TRACK_NODE) {
    n->reason = GOT_LOST;
    return true;
  }

  tmemcpy(&n->subject.loc, &c->loc, sizeof(c->loc));

  for (int i = 0; i < MAX_LOCATIONS_TO_OBSERVE; i++) {
    if (is_location_set[i]
        && location_is_ge(&c->loc, &locations_to_observe[i].subject.loc)) {
      n->reason = locations_to_observe[i].reason;
      tmemcpy(n->action.switch_to_switch, locations_to_observe[i].action.switch_to_switch,
          sizeof(n->action.switch_to_switch));
      is_location_set[i] = false;
      return true;
    }
  }

  if (c->loc.node != last->loc.node) {
    n->reason = LOCATION_CHANGED;
    return true;
  }

  return false; // the boring case -- nothing happened!
}

int add_notification_requests(
        location_notification notifications[MAX_LOCATIONS_TO_OBSERVE],
        int n_requests,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE]) {
  Assert(n_requests <= MAX_LOCATIONS_TO_OBSERVE);
  int j = 0;
  for (int i = 0; i < n_requests; i++) {
#if DEBUG_TRAIN_COORDINATOR
    // logprintf("Adding request of type %d (%s +- %d)\n\r", notifications[i].reason, notifications[i].loc.node->name, notifications[i].loc.offset);
#endif /* DEBUG_TRAIN_COORDINATOR */
    Assert(notifications[i].reason != LOCATION_ANY);
    if (j >= MAX_LOCATIONS_TO_OBSERVE) {
      return TOO_MANY_NOTIFICATION_REQUESTS;
    }
    bool is_dup = false;
    bool override = false;
    for (int k = 0; k < MAX_LOCATIONS_TO_OBSERVE; k++) {
      if (is_location_set[k]) {
        is_dup |= notifications[i].subject.loc.node == locations_to_observe[k].subject.loc.node
          && ABS(notifications[i].subject.loc.offset - locations_to_observe[k].subject.loc.offset) < 300;
        if (locations_to_observe[k].reason == LOCATION_TO_STOP
              && notifications[i].reason == LOCATION_TO_STOP) {
          is_location_set[k] = false;
          override = true;
          // If they're the same switch w/ the same configuration, then drop existing
        } else if (locations_to_observe[k].reason == LOCATION_TO_SWITCH
              && notifications[i].reason == LOCATION_TO_SWITCH
              && locations_to_observe[k].action.switch_to_switch[0] == notifications[i].action.switch_to_switch[0]
              && locations_to_observe[k].action.switch_to_switch[1] == notifications[i].action.switch_to_switch[1]) {
          is_location_set[k] = false;
          override = true;
        }
        if (is_dup && !override) break;
      }
    }
    if (is_dup && !override) continue;
    for (; j < MAX_LOCATIONS_TO_OBSERVE; j++) {
      if (!is_location_set[j]) {
        is_location_set[j] = true;
        tmemcpy(locations_to_observe + j, notifications + i,
            sizeof(notifications[i]));
        break;
      }
    }
  }
  return 0;
}

void drop_all_notifications(bool is_location_set[MAX_LOCATIONS_TO_OBSERVE]) {
  for (int i = 0; i < MAX_LOCATIONS_TO_OBSERVE; i++) {
    is_location_set[i] = false;
  }
}

void coordinate_courier() {
  int conductor = MyParentTid();
  int track_state_controller = WhoIs("TrackStateController");
  int coordinate_server = WhoIs("TrainCoordinatesServer");
  int clock_server = WhoIs("ClockServer");
  coordinates c, last;
  last.loc.node = NULL_TRACK_NODE;

  int message_tid;
  message train_msg;
  Assert(Receive(&message_tid, &train_msg, sizeof(train_msg))
      == sizeof(train_msg));
  Assert(message_tid == conductor);
  Assert(Reply(message_tid, EMPTY_MESSAGE, 0) == 0);
  int train = train_msg.msg.train;

  location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE];
  // Request: Ask what to observe
  // Response: The thing we really observed
  message n_request, n_observed;
  n_observed.type = MESSAGE_CONDUCTOR_NOTIFY_REQUEST;
  bool is_location_set[MAX_LOCATIONS_TO_OBSERVE];
  bool should_find_any = false;
  bool first_run = true;
  bool is_blocked = false;
  drop_all_notifications(is_location_set);
  while (true) {
    get_coordinates(coordinate_server, train, &c);
    bool got_not = coordinates_to_notification(&c, &last, locations_to_observe,
                                is_location_set,
                                &n_observed.msg.notification_response);
    bool has_fresh_loss = c.loc.node == NULL_TRACK_NODE && last.loc.node != NULL_TRACK_NODE;
    if ((should_find_any && c.loc.node != NULL_TRACK_NODE) || first_run || (got_not && (c.loc.node != NULL_TRACK_NODE || has_fresh_loss))) {
      if (has_fresh_loss) {
        should_find_any = true;
      }
      has_fresh_loss = false;
      if (should_find_any && c.loc.node != NULL_TRACK_NODE) {
        tmemcpy(&n_observed.msg.notification_response.subject.loc, &c, sizeof(c));
        n_observed.msg.notification_response.reason = LOCATION_ANY;
        should_find_any = false;
      }
      if (first_run) {
        n_observed.msg.notification_response.reason = c.loc.node == NULL_TRACK_NODE ?
                                                      GOT_LOST :
                                                      LOCATION_ANY;
        first_run = false;
      }
      Assert(Send(conductor, &n_observed, sizeof(n_observed),
                             &n_request, sizeof(n_request)) == sizeof(n_request));
      tmemcpy(&last, &c, sizeof(c));
      Assert(n_request.type == REPLY_CONDUCTOR_NOTIFY_REQUEST);
      if (n_request.msg.notification_request.drop_existing) {
        is_blocked = false;
        drop_all_notifications(is_location_set);
      }
      int r = add_notification_requests(
                  n_request.msg.notification_request.notifications,
                  n_request.msg.notification_request.num_requests,
                  locations_to_observe, is_location_set);
      Assert(r != TOO_MANY_NOTIFICATION_REQUESTS);
    }
    
    // Check if we would collide within one stopping distance
    coordinates group_coordinates[MAX_GROUP_MEMBERS];
    // For now, just assume that we only have one group
    train_group *g = &tr_groups[0].g;
    for (int i = 0; i < g->num_members; i++) {
      int t = g->members[i];
      get_coordinates(coordinate_server, t, &group_coordinates[i]);
    }
     // TODO get_stopping_distance_model(track_state_controller, train, stopping_distance_model);
    int sd = 170000; // For now just eyeballing
    turnout_state turnout_states[NUM_TURNOUTS];
    get_turnouts(track_state_controller, turnout_states);
    int max_speed = -1;
    n_observed.msg.notification_response.action.distance[1] = c.current_speed;
    bool will_collide = will_collide_with_other_train(sd, &c, group_coordinates,
                                  g->num_members, turnout_states, &max_speed,
                                  train);
    is_blocked = will_collide;
    if (will_collide) {
      // TODO don't send this too often
      n_observed.msg.notification_response.subject.trains[0] = train;
      n_observed.msg.notification_response.reason = LOCATION_SLOWDOWN;
      n_observed.msg.notification_response.action.distance[0] = max_speed;
      Assert(Send(conductor, &n_observed, sizeof(n_observed),
                             EMPTY_MESSAGE, 0) == 0);
    }
    Delay(clock_server, 1);
  }
  Assert(0);
}

int create_courier(int train) {
  int coordinate_courier_tid = Create(MyPriority(), &coordinate_courier);
  message train_message;
  train_message.msg.train = train;
  Assert(coordinate_courier_tid >= 0);
  Assert(Send(coordinate_courier_tid, &train_message, sizeof(train_message),
              EMPTY_MESSAGE, 0)
          == 0);
  return coordinate_courier_tid;
}

/*
void send_notification_request(int coordinate_courier_tid,
                               location_notification requests[MAX_LOCATIONS_TO_OBSERVE],
                               int n_requests) {
  message send;
  tmemcpy(&send.msg.notification_request.notifications, requests,
          sizeof(requests[0]) * n_requests);
  send.msg.notification_request.num_requests = n_requests;
  Assert(Send(coordinate_courier_tid, &send, sizeof(send), EMPTY_MESSAGE, 0)
      == 0);
}

*/
