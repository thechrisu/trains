#include "coordinate_courier.h"

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

void coordinate_courier() {
  int conductor = MyParentTid();
  int coordinate_server = WhoIs("TrainCoordinateServer");
  int train; // TODO get train via initial message from parent.
  coordinates c;

  int message_tid;
  (void)message_tid;

  location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE];
  message n_request, n_response;
  n_request.type = MESSAGE_CONDUCTOR_NOTIFY_REQUEST;
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
    // TODO process new request
  }
  Assert(0);
}
