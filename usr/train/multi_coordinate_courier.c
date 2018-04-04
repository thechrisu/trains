#include "multi_coordinate_courier.h"

#define TOO_MANY_NOTIFICATION_REQUESTS -1

#define SPACING_NOTIFICATION_PERIOD 25

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define ABS(a) ((a) < 0 ? -(a) : (a))

/**
 * Helper function, useful to diagnose problems arising from a starved notification.
 * (So far, we haven't had any problems tho)
 *
 * @Param locations_to_observe             Possible notification slows.
 * @param is_location_set                  Map to valid notification slots.
 * @param c                                Our location + speed.
 */
void multi_print_num_triggerable_notifications(
          location_notification locations_to_observe[MULTI_MAX_LOCATIONS_TO_OBSERVE],
          bool is_location_set[MULTI_MAX_LOCATIONS_TO_OBSERVE],
          coordinates *c) {
  int n_notifications = 0;
  for (int i = 0; i < MULTI_MAX_LOCATIONS_TO_OBSERVE; i++) {
    if (is_location_set[i]
        && location_is_ge(&c->loc, &locations_to_observe[i].subject.loc)) {
      n_notifications += 1;
    }
  }
  if (n_notifications > 1) {
    logprintf("Notifications: %d\n\r", n_notifications);
  }
}

bool multi_coordinates_to_notification(coordinates *c, coordinates *last,
        location_notification locations_to_observe[MULTI_MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MULTI_MAX_LOCATIONS_TO_OBSERVE],
        location_notification *n) {
  if (c->loc.node == NULL_TRACK_NODE) {
    n->reason = GOT_LOST;
    return true;
  }

  tmemcpy(&n->subject.loc, &c->loc, sizeof(c->loc));

  for (int i = 0; i < MULTI_MAX_LOCATIONS_TO_OBSERVE; i++) {
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

int multi_add_notification_requests(
        location_notification notifications[MULTI_MAX_LOCATIONS_TO_OBSERVE],
        int n_requests,
        location_notification locations_to_observe[MULTI_MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MULTI_MAX_LOCATIONS_TO_OBSERVE]) {
  Assert(n_requests <= MULTI_MAX_LOCATIONS_TO_OBSERVE);
  int j = 0;
  for (int i = 0; i < n_requests; i++) {
#if DEBUG_TRAIN_COORDINATOR
    // logprintf("Adding request of type %d (%s +- %d)\n\r", notifications[i].reason, notifications[i].loc.node->name, notifications[i].loc.offset);
#endif /* DEBUG_TRAIN_COORDINATOR */
    Assert(notifications[i].reason != LOCATION_ANY);
    if (j >= MULTI_MAX_LOCATIONS_TO_OBSERVE) {
      return TOO_MANY_NOTIFICATION_REQUESTS;
    }
    bool is_dup = false;
    bool override = false;
    for (int k = 0; k < MULTI_MAX_LOCATIONS_TO_OBSERVE; k++) {
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
    for (; j < MULTI_MAX_LOCATIONS_TO_OBSERVE; j++) {
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

void multi_drop_all_notifications(bool is_location_set[MULTI_MAX_LOCATIONS_TO_OBSERVE]) {
  for (int i = 0; i < MULTI_MAX_LOCATIONS_TO_OBSERVE; i++) {
    is_location_set[i] = false;
  }
}

/**
 * @param   in_front       The coordinates of the train in front.
 * @param   behind         The coordinates of the train behind.
 * @param   trains_between The number of trains between the two trains.
 * @returns The spacing between two trains in millimetres, as if they followed
 *          each other directly, but given that in reality there are
 *          `trains_between` trains in between them (which we assume are
 *          perfectly spaced).
 *          -1 if exactly one of the trains is lost.
 */
int get_effective_spacing(coordinates *in_front,
                          coordinates *behind,
                          int trains_between) {
  if (in_front->loc.node == NULL_TRACK_NODE &&
      behind->loc.node == NULL_TRACK_NODE) {
    return spacing + (in_front->loc.offset - behind->loc.offset) / 100;
  }

  int actual_distance = distance_between_locations(&behind->loc,
                                                   &in_front->loc);
  if (actual_distance < 0) return actual_distance;

  int distance_between_pickups = spacing * 100 + TRAIN_LENGTH;
  int effective_distance = actual_distance -
                           distance_between_pickups * trains_between;
  return (effective_distance - TRAIN_LENGTH) / 100;
}


void multi_coordinate_courier() {
  int conductor = MyParentTid();
  int coordinate_server = WhoIs("TrainCoordinatesServer");
  int clock_server = WhoIs("ClockServer");
  coordinates c, last; // TODO update this for groups
  last.loc.node = NULL_TRACK_NODE;

  int message_tid;
  message group_msg;
  Assert(Receive(&message_tid, &group_msg, sizeof(group_msg))
      == sizeof(group_msg));
  Assert(message_tid == conductor);
  Assert(Reply(message_tid, EMPTY_MESSAGE, 0) == 0);
  train_group group;
  tmemcpy(&group, &group_msg.msg.group_content, sizeof(group));

  int last_spacing_notification[group.num_members];
  for (int i = 0; i < group.num_members; i += 1) {
    last_spacing_notification[i] = -(SPACING_NOTIFICATION_PERIOD + 1);
  }

  location_notification locations_to_observe[MULTI_MAX_LOCATIONS_TO_OBSERVE];
  // Request: Ask what to observe
  // Response: The thing we really observed
  message n_request, n_observed;
  n_observed.type = MESSAGE_CONDUCTOR_NOTIFY_REQUEST;
  bool is_location_set[MULTI_MAX_LOCATIONS_TO_OBSERVE];
  bool should_find_any = false;
  bool first_run = true;
  multi_drop_all_notifications(is_location_set);
  while (true) {
    // Use notifications for all trains
    // Actually, it's ok only to use notifications for the first train
    for (int i = 0; i < group.num_members && i < 1; i++) {
      int train = group.members[i];
      get_coordinates(coordinate_server, train, &c);
      bool got_not = multi_coordinates_to_notification(&c, &last,
                                  locations_to_observe, is_location_set,
                                  &n_observed.msg.notification_response);
      bool has_fresh_loss = c.loc.node == NULL_TRACK_NODE && last.loc.node != NULL_TRACK_NODE;
      if ((should_find_any && c.loc.node != NULL_TRACK_NODE) || first_run
          || (got_not && (c.loc.node != NULL_TRACK_NODE || has_fresh_loss))) {
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
                                                       GOT_LOST : LOCATION_ANY;
        first_run = false;
        }
        Assert(Send(conductor, &n_observed, sizeof(n_observed),
                               &n_request, sizeof(n_request)) == sizeof(n_request));
        tmemcpy(&last, &c, sizeof(c));
        Assert(n_request.type == REPLY_CONDUCTOR_NOTIFY_REQUEST);
        if (n_request.msg.notification_request.drop_existing) {
          multi_drop_all_notifications(is_location_set);
        }
        int r = multi_add_notification_requests(
                    n_request.msg.notification_request.notifications,
                  n_request.msg.notification_request.num_requests,
                  locations_to_observe, is_location_set);
        Assert(r != TOO_MANY_NOTIFICATION_REQUESTS);
      }

    }

    coordinates first;
    get_coordinates(coordinate_server, group.members[0], &first);

    int time = Time(clock_server);

    // Check spacing
    for (int i = 0; i < group.num_members - 1; i++) {
      int leader_t = group.members[i];
      int follower_t = group.members[i + 1];
      coordinates leader, follower; // Leader: Train AHEAD of follower
      get_coordinates(coordinate_server, leader_t, &leader);
      get_coordinates(coordinate_server, follower_t, &follower);

      int to_first = get_effective_spacing(&first, &follower, i);
      int to_leader = get_effective_spacing(&leader, &follower, 0);
      int d = MAX(to_first, to_leader);
      if (d > 0 &&
          (d > spacing + spacing_error || d < spacing - spacing_error) &&
          last_spacing_notification[i] < time - SPACING_NOTIFICATION_PERIOD) {
        last_spacing_notification[i] = time;

        n_observed.msg.notification_response.subject.trains[0] = leader_t;
        n_observed.msg.notification_response.subject.trains[1] = follower_t;
        n_observed.msg.notification_response.reason = SPACING;
        n_observed.msg.notification_response.action.distance[0] = d;
        n_observed.msg.notification_response.action.distance[1] = spacing;
        Assert(Send(conductor, &n_observed, sizeof(n_observed),
                               EMPTY_MESSAGE, 0) == 0);
      }
    }
    Delay(clock_server, 1);
  }
  Assert(0);
}

int create_multi_courier(train_group *group) {
  int coordinate_courier_tid = Create(MyPriority(), &multi_coordinate_courier);
  message group_message;
  tmemcpy(&group_message.msg.group_content, group,
          sizeof(group_message.msg.group_content));
  Assert(coordinate_courier_tid >= 0);
  Assert(Send(coordinate_courier_tid, &group_message, sizeof(group_message),
              EMPTY_MESSAGE, 0)
          == 0);
  return coordinate_courier_tid;
}
