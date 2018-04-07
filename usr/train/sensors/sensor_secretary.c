#include "sensor_secretary.h"

void sensor_secretary() {
  int train_tx_server = WhoIs("TrainTxServer");
  int train_rx_server = WhoIs("TrainRxServer");
  int event_server = WhoIs("EventServer");
  int sensor_interpreter = WhoIs("SensorInterpreter");

  Assert(train_tx_server > 0);
  Assert(train_rx_server > 0);
  Assert(event_server > 0);
  Assert(sensor_interpreter > 0);

  bool first_query = true;

  while (true) {
    Assert(Putc(train_tx_server, TRAIN, CMD_ALL_SENSORS) == 0);

    message sensor_msg;
    sensor_msg.type = MESSAGE_SENSORSRECEIVED;

    event sensors;
    sensors.type = EVENT_SENSOR_DATA_RECEIVED;

    for (int i = 0; i < 10; i++) {
      char c = Getc(train_rx_server, TRAIN);
      sensor_msg.msg.sensors[i] = c;
      sensors.body.sensors[i] = c;
    }

    if (!first_query) {
      Publish(event_server, &sensors);
      Assert(Send(sensor_interpreter, &sensor_msg, sizeof(sensor_msg), EMPTY_MESSAGE, 0) == 0);
    }

    first_query = false;
  }
}
