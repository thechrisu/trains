#include "sensor_view.h"

#define SENSOR_HISTORY_LENGTH 10
#define SENSOR_HEADING_LINE 4
#define SENSOR_REFRESH_PERIOD 6

static char_buffer *recent_sensors_buf;

char get_sensor_index(int receive_index, int sensor_offset);

void print_sensors(int terminal_tx_server, int16_t sensors[10]) {
  bool val_changed = false;
  int32_t old_offset = SENSOR_HEADING_LINE + 1 + (recent_sensors_buf->in == 0 ? recent_sensors_buf->elems : recent_sensors_buf->in - 1);
  for (int i = 0; i < 10; i++) {
    // logprintf("Sensor view: %x", sensors[i]);
    if (sensors[i]) {
      for (int j = 0; j < 8; j++) {
        char mask = 1 << (7 - j);
        if (sensors[i] & mask) {
          val_changed = true;
          char ltr = 'A' + (i/2);
          char num = 1 + j + (i % 2 == 0 ? 0 : 8);
          Printf(terminal_tx_server, "%s%d;%dH%c%d   ", ESC, SENSOR_HEADING_LINE + 1 + recent_sensors_buf->in, 1, ltr, num);
          char_buffer_put_replace(recent_sensors_buf, get_sensor_index(i, j));
        }
      }
    }
  }
  if (val_changed) {
    Printf(terminal_tx_server, "%s%d;%dH   ", ESC, old_offset, 6);
    Printf(terminal_tx_server, "%s%d;%dH<--", ESC, SENSOR_HEADING_LINE + 1 + (recent_sensors_buf->in == 0 ? recent_sensors_buf->elems : recent_sensors_buf->in - 1), 6);
  }
}

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]) {
  for (int i = 0; i  < 10; i++) {
    leading_edge[i] = ~old_sensors[i] & new_sensors[i]; // 0 -> 1
  }
}

char get_sensor_index(int receive_index, int sensor_offset) {
  char sensor_letter_offset = receive_index / 2;
  return sensor_letter_offset * 16 + sensor_offset + ((receive_index % 2 == 0) ? 0 : 8);
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
    Assert(Delay(clock_server, SENSOR_REFRESH_PERIOD) == 0);
    message sensor_poll_msg, sensor_poll_reply;
    sensor_poll_msg.type = MESSAGE_GETSENSORS;
    Assert(Send(track_state_controller,
         &sensor_poll_msg, sizeof(sensor_poll_msg),
                &sensor_poll_reply, sizeof(sensor_poll_reply)) > 0);
    get_leading_edge(current_sensors, sensor_poll_reply.msg.sensors, leading_edge);
    print_sensors(terminal_tx_server, leading_edge);
  }
}

