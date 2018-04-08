#include "sensor_view.h"

#define SENSOR_HISTORY_LENGTH 10

static char_buffer *recent_sensors_buf;

void print_sensor(int terminal_tx_server, char sensor) {
  uint32_t offset = recent_sensors_buf->in;
  int line_to_print = SENSOR_HEADING_LINE + 1 + offset;
  int line_to_clear = SENSOR_HEADING_LINE + 1 + (offset + 9) % 10;

  Assert(Printf(terminal_tx_server, "%s%d;%dH%c%d   ",
                ESC, line_to_print, 1,
                sensor_bank(sensor), sensor_index(sensor)) == 0);
  Assert(Printf(terminal_tx_server, "%s%d;%dH<--",
                ESC, line_to_print, 6) == 0);
  Assert(Printf(terminal_tx_server, "%s%d;%dH   ",
                ESC, line_to_clear, 6) == 0);

  char_buffer_put_replace(recent_sensors_buf, sensor);
}

void sensor_view() {
  int terminal_tx_server = WhoIs("TerminalTxServer");
  int event_broker = WhoIs("EventBroker");

  Assert(terminal_tx_server > 0);
  Assert(event_broker > 0);

  Subscribe(event_broker, EVENT_SENSOR_TRIGGERED);

  char recent_sensors[SENSOR_HISTORY_LENGTH];
  char_buffer recent_sensors_buf_loc;
  char_buffer_init(&recent_sensors_buf_loc, recent_sensors, SENSOR_HISTORY_LENGTH);
  recent_sensors_buf = &recent_sensors_buf_loc;

  Assert(Printf(terminal_tx_server,
                "%s%d;%dHSensors: ", ESC, SENSOR_HEADING_LINE, 1) == 0);

  event e;

  while (true) {
    ReceiveEvent(&e);
    Assert(e.type == EVENT_SENSOR_TRIGGERED);
    print_sensor(terminal_tx_server, e.body.sensor);
  }
}
