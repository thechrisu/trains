#include "sensor_secretary.h"

void sensor_secretary() {
  int train_tx_server = WhoIs("TrainTxServer");
  int train_rx_server = WhoIs("TrainRxServer");
  int track_state_controller = WhoIs("TrackStateController");
  Assert(train_tx_server > 0);
  Assert(train_rx_server > 0);
  Assert(track_state_controller > 0);
  while (true) {
    Assert(Putc(train_tx_server, TRAIN, CMD_ALL_SENSORS) == 0);
    uint16_t sensors[10];
    for (int i = 0; i < 10; i++) {
      int c = Getc(train_rx_server, TRAIN);
      Assert(c >= 0);
      sensors[i] = c;
    }
    // TODO push to track state controller. 
  }
}
