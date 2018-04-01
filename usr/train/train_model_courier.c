#include "train_model_courier.h"

#define TRAIN_MODEL_COURIER_REFRESH_PERIOD 200 // Ticks

void train_model_courier() {
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");

  while (true) {
    for (int i = 0; i < num_active_trains; i++) {
      int train = active_trains[i];
      message send;
      send.type = MESSAGE_REQUEST_COORD_UPDATE;
      send.msg.train = train;
      Assert(Send(track_state_controller, &send, sizeof(send), EMPTY_MESSAGE, 0)
          == 0);
    }
    Delay(clock_server, TRAIN_MODEL_COURIER_REFRESH_PERIOD);
  }
}
