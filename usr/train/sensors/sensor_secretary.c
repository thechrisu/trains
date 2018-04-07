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
      int c = Getc(train_rx_server, TRAIN);
      Assert(c >= 0);
      sensor_msg.msg.sensors[i] = c;
    }

    int16_t leading_edge[10];
    get_leading_edge(sensor_states, sensor_msg.msg.sensors, leading_edge);

    if (!first_query) {
      for (unsigned int i = 0; i < 80; i += 1) {
        if (leading_edge[sensor_data_element(i)] & sensor_data_mask(i)) {
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
