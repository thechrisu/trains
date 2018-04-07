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

  int16_t sensor_states[10];
  tmemset(sensor_states, 0, sizeof(sensor_states));

  while (true) {
    Assert(Putc(train_tx_server, TRAIN, CMD_ALL_SENSORS) == 0);

    message sensor_msg;
    sensor_msg.type = MESSAGE_SENSORSRECEIVED;

    for (int i = 0; i < 10; i++) {
      sensor_msg.msg.sensors[i] = Getc(train_rx_server, TRAIN);
    }

    int16_t leading_edge[10];
    get_leading_edge(sensor_states, sensor_msg.msg.sensors, leading_edge);

    if (!first_query) {
      for (int i = 0; i < 80; i += 1) {
        int index = i / 8;
        int bit = (15 - (i % 16)) % 8;

        if (leading_edge[index] & (1 << bit)) {
          event e = {
            .type = EVENT_SENSOR_TRIGGERED,
            .body = { .sensor = i },
          };
          Publish(event_server, &e);
        }
      }

      Assert(Send(sensor_interpreter, &sensor_msg, sizeof(sensor_msg), EMPTY_MESSAGE, 0) == 0);
    }

    first_query = false;
  }
}
