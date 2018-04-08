#include "sensor_secretary.h"

void sensor_secretary() {
  int train_tx_server = WhoIs("TrainTxServer");
  int train_rx_server = WhoIs("TrainRxServer");
  int event_broker = WhoIs("EventBroker");

  Assert(train_tx_server > 0);
  Assert(train_rx_server > 0);
  Assert(event_broker > 0);

  bool first_query = true;

  int16_t sensor_states[10];
  tmemset(sensor_states, 0, sizeof(sensor_states));

  while (true) {
    Assert(Putc(train_tx_server, TRAIN, CMD_ALL_SENSORS) == 0);

    int16_t new_sensor_states[10];

    for (int i = 0; i < 10; i++) {
      int c = Getc(train_rx_server, TRAIN);
      Assert(c >= 0);
      new_sensor_states[i] = c;
    }

    int16_t leading_edge[10];
    get_leading_edge(sensor_states, new_sensor_states, leading_edge);

    if (!first_query) {
      for (unsigned int i = 0; i < 80; i += 1) {
        if (leading_edge[sensor_data_element(i)] & sensor_data_mask(i)) {
          event e = {
            .type = EVENT_SENSOR_TRIGGERED,
            .body = { .sensor = i },
          };
          Publish(event_broker, &e);
        }
      }
    }

    first_query = false;
  }
}
