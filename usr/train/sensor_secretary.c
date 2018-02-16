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
    logprintf("PUT sensor\n\r");
    message sensor_msg;
    sensor_msg.type = MESSAGE_SENSORSRECEIVED;
    for (int i = 0; i < 10; i++) {
      int c = Getc(train_rx_server, TRAIN);
      logprintf("Sensor sec (%d)\n\r", i + 1);
      Assert(c >= 0);
      sensor_msg.msg.sensors[i] = c;
    }
    Send(track_state_controller, &sensor_msg, sizeof(sensor_msg), EMPTY_MESSAGE, 0);
  }
}
