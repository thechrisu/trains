#include "coordinate_courier.h"

#define TOO_MANY_NOTIFICATION_REQUESTS -1

// returns true if they're the same node, but a's offset is >= b's offset
bool location_is_ge(location *a, location *b) {
  if (node_follows(a->node, b->node)) {
    return true;
  }
  return a->node == b->node && a->offset >= b->offset;
}

bool coordinates_to_notification(coordinates *c, coordinates *last,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE],
        location_notification *n) {
  if (c->loc.node == NULL_TRACK_NODE) {
    n->reason = GOT_LOST;
    return true;
  }
  tmemcpy(&n->loc, &c->loc, sizeof(c->loc));
  for (int i = 0; i < MAX_LOCATIONS_TO_OBSERVE; i++) {
    if (is_location_set[i]
        && location_is_ge(&c->loc, &locations_to_observe[i].loc)) {
      n->reason = locations_to_observe[i].reason;
      tmemcpy(n->switch_to_switch, locations_to_observe[i].switch_to_switch,
          sizeof(n->switch_to_switch));
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
    Assert(notifications[i].reason != LOCATION_ANY);
    if (j >= MAX_LOCATIONS_TO_OBSERVE) {
      return TOO_MANY_NOTIFICATION_REQUESTS;
    }
    bool is_dup = false;
    for (int k = 0; k < MAX_LOCATIONS_TO_OBSERVE; k++) {
      if (is_location_set[k]) {
        is_dup |= notifications[i].loc.node == locations_to_observe[k].loc.node
          && notifications[i].loc.offset == locations_to_observe[k].loc.offset;
        if (locations_to_observe[k].reason == LOCATION_TO_STOP
              && notifications[i].reason == LOCATION_TO_STOP) {
          is_location_set[k] = false;
          // If they're the same switch w/ the same configuration, then drop existing
        } else if (locations_to_observe[k].reason == LOCATION_TO_SWITCH
              && notifications[i].reason == LOCATION_TO_SWITCH
              && locations_to_observe[k].switch_to_switch[0] == notifications[i].switch_to_switch[0]
              && locations_to_observe[k].switch_to_switch[1] == notifications[i].switch_to_switch[1]) {
          is_location_set[k] = false;
        }
        if (is_dup) break;
      }
    }
    if (is_dup) continue;
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
  int coordinate_server = WhoIs("TrainCoordinateServer");
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
  drop_all_notifications(is_location_set);
  while (true) {
    get_coordinates(coordinate_server, train, &c);
    bool got_not = coordinates_to_notification(&c, &last, locations_to_observe,
                                is_location_set,
                                &n_observed.msg.notification_response);
    if ((should_find_any && c.loc.node != NULL_TRACK_NODE) || got_not) {
      if (should_find_any && c.loc.node != NULL_TRACK_NODE) {
          tmemcpy(&n_observed.msg.notification_response.loc, &c, sizeof(c));
          n_observed.msg.notification_response.reason = LOCATION_ANY;
      }
      Assert(Send(conductor, &n_observed, sizeof(n_observed),
                  &n_request, sizeof(n_request)
                                 == sizeof(n_request)));
      tmemcpy(&last, &c, sizeof(c));
      Assert(n_request.type == REPLY_CONDUCTOR_NOTIFY_REQUEST);
      if (n_request.msg.notification_request.drop_existing) {
        drop_all_notifications(is_location_set);
      }
      int r = add_notification_requests(
                  n_request.msg.notification_request.notifications,
                  n_request.msg.notification_request.num_requests,
                  locations_to_observe, is_location_set);
      Assert(r != TOO_MANY_NOTIFICATION_REQUESTS);
    }
    Delay(clock_server, 1);
  }
  Assert(0);
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
