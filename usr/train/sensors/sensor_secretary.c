#include "sensor_secretary.h"

void sensor_secretary() {
  int train_tx_server = WhoIs("TrainTxServer");
  int train_rx_server = WhoIs("TrainRxServer");
  int track_state_controller = WhoIs("TrackStateController");
  int sensor_interpreter = WhoIs("SensorInterpreter");
  Assert(train_tx_server > 0);
  Assert(train_rx_server > 0);
  Assert(track_state_controller > 0);
  Assert(sensor_interpreter > 0);

  bool first_query = true;

  while (true) {
    Assert(Putc(train_tx_server, TRAIN, CMD_ALL_SENSORS) == 0);
    message sensor_msg;
    sensor_msg.type = MESSAGE_SENSORSRECEIVED;
    for (int i = 0; i < 10; i++) {
      int c = Getc(train_rx_server, TRAIN);
      Assert(c >= 0);
      sensor_msg.msg.sensors[i] = c;
    }

    if (!first_query) {
      Assert(Send(track_state_controller, &sensor_msg, sizeof(sensor_msg), EMPTY_MESSAGE, 0) == 0);
      Assert(Send(sensor_interpreter, &sensor_msg, sizeof(sensor_msg), EMPTY_MESSAGE, 0) == 0);
    }

    first_query = false;
  }
}
