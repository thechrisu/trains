#include "train_coordinates_server.h"

void train_coordinates_server() {
  Assert(RegisterAs("TrainCoordinatesServer") == 0);

  int sender_tid;
  message received, reply;

  (void)reply;

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    switch (received.type) {
      case MESSAGE_TRAINSETSPEED:
        break;
      case MESSAGE_TRAINREVERSED:
        break;
      case REPLY_GET_LAST_SENSOR_HIT:
        break;
      case MESSAGE_GET_COORDINATES:
        break;
      default:
        logprintf("Received message of type %d in train coordinates server\n\r", received.type);
        Assert(0);
    }
  }
}
