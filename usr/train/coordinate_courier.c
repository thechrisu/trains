#include "coordinate_courier.h"

#define MAX_LOCATIONS_TO_OBSERVE 10

void coordinate_courier() {
  int my_priority = MyPriority();
  int coordinate_server = WhoIs("TrainCoordinateServer");

  int message_tid;
  message coord_message, coord_reply;

  location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE];
  while (true) {
    Assert(Send(coordinate_server, &coord_message, sizeof(coord_message),
                                   &coord_reply, sizeof(coord_reply)
                               == sizeof(coord_reply)));
    // TODO check whether that matches any of the locations we should observe
    Assert(Send(coordinate_server, &coord_message, sizeof(coord_message),
                                   &coord_reply, sizeof(coord_reply)
                               == sizeof(coord_reply)));
  }
  Assert(0);
}
