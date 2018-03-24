#include "coordinate_courier.h"

#define TOO_MANY_NOTIFICATION_REQUESTS -1

void coordinates_to_notification(coordinates *c,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE],
        location_notification *n) {
  if (c->loc.node == NULL_TRACK_NODE) {
    n->reason = GOT_LOST;
    // GOT LOST
  } else {
    n->node = c->loc.node;
    for (int i = 0; i < MAX_LOCATIONS_TO_OBSERVE; i++) {
      if (is_location_set[i]
          && locations_to_observe[i].node == c->loc.node) {
        n->reason = locations_to_observe[i].reason;
        tmemcpy(n->switch_to_switch, locations_to_observe[i].switch_to_switch,
            sizeof(n->switch_to_switch));
        is_location_set[i] = false;
        return;
      }
    }
    n->reason = LOCATION_CHANGED;
  }
}

int add_notification_requests(
        location_notification notifications[MAX_LOCATIONS_TO_OBSERVE],
        int n_requests,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE]) {
  Assert(n_requests <= MAX_LOCATIONS_TO_OBSERVE);
  int j = 0;
  for (int i = 0; i < n_requests; i++) {
    if (j >= MAX_LOCATIONS_TO_OBSERVE) {
      return TOO_MANY_NOTIFICATION_REQUESTS;
    }
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

void coordinate_courier() {
  int conductor = MyParentTid();
  int coordinate_server = WhoIs("TrainCoordinateServer");
  coordinates c;

  int message_tid;
  message train_msg;
  Assert(Receive(&message_tid, &train_msg, sizeof(train_msg))
      == sizeof(train_msg));
  Assert(Reply(message_tid, EMPTY_MESSAGE, 0) == 0);
  int train = train_msg.msg.train;

  location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE];
  message n_request, n_response;
  n_response.type = REPLY_CONDUCTOR_NOTIFY_REQUEST;
  bool is_location_set[MAX_LOCATIONS_TO_OBSERVE];
  for (int i = 0; i < MAX_LOCATIONS_TO_OBSERVE; i++) {
    is_location_set[i] = false;
  }
  while (true) {
    get_coordinates(coordinate_server, train, &c);
    coordinates_to_notification(&c, locations_to_observe, is_location_set,
                                &n_response.msg.notification_response);
    Assert(Send(conductor, &n_response, sizeof(n_response),
                &n_request, sizeof(n_request)
                               == sizeof(n_request)));

    Assert(n_request.type == MESSAGE_CONDUCTOR_NOTIFY_REQUEST);
    int r = add_notification_requests(
                n_response.msg.notification_request.notifications,
                n_response.msg.notification_request.num_requests,
                locations_to_observe, is_location_set);
    Assert(r != TOO_MANY_NOTIFICATION_REQUESTS);
  }
  Assert(0);
}
