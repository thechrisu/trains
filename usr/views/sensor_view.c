#include "sensor_view.h"

#define SENSOR_HISTORY_LENGTH 10

static char_buffer *recent_sensors_buf;

void print_sensors(int terminal_tx_server, int16_t sensors[10]) {
  for (int i = 0; i < 80; i += 1) {
    if (is_sensor_triggered(sensors, i)) {
      char ltr = sensor_bank(i);
      unsigned int num = sensor_index(i);
      uint32_t offset = recent_sensors_buf->in;
      Printf(terminal_tx_server, "%s%d;%dH%c%d   ", ESC, SENSOR_HEADING_LINE + 1 + offset, 1, ltr, num);
      Printf(terminal_tx_server, "%s%d;%dH   ", ESC, SENSOR_HEADING_LINE + 1 + (offset + 9) % 10, 6);
      char_buffer_put_replace(recent_sensors_buf, offset);
      Printf(terminal_tx_server, "%s%d;%dH<--", ESC, SENSOR_HEADING_LINE + 1 + (recent_sensors_buf->in == 0 ? recent_sensors_buf->elems - 1 : recent_sensors_buf->in - 1), 6);
    }
  }
}

void sensor_view() {
  int terminal_tx_server = WhoIs("TerminalTxServer");
  int track_state_controller = WhoIs("TrackStateController");
  int clock_server = WhoIs("ClockServer");

  Assert(terminal_tx_server > 0);
  Assert(track_state_controller > 0);
  Assert(clock_server > 0);

  char recent_sensors[SENSOR_HISTORY_LENGTH];
  char_buffer recent_sensors_buf_loc;
  char_buffer_init(&recent_sensors_buf_loc, recent_sensors, SENSOR_HISTORY_LENGTH);
  recent_sensors_buf = &recent_sensors_buf_loc;

  Assert(track_state_controller > 0);
  int16_t current_sensors[10];
  int16_t leading_edge[10];
  tmemset(current_sensors, 0, sizeof(current_sensors));
  tmemset(leading_edge, 0, sizeof(leading_edge));

  Printf(terminal_tx_server, "%s%d;%dHSensors: ", ESC, SENSOR_HEADING_LINE, 1);
  while (true) {
    Assert(Delay(clock_server, REFRESH_PERIOD) == 0);
    message sensor_poll_reply;
    get_sensors(track_state_controller, &sensor_poll_reply);
    get_leading_edge(current_sensors, sensor_poll_reply.msg.sensors, leading_edge);
    print_sensors(terminal_tx_server, leading_edge);
  }
}

