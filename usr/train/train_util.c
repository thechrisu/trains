#include "train_util.h"

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]) {
  for (int i = 0; i  < 10; i++) {
    leading_edge[i] = ~old_sensors[i] & new_sensors[i]; // 0 -> 1
    old_sensors[i] = new_sensors[i];
  }
}

bool is_sensor_triggered(int16_t sensors[10], unsigned int offset) {
  return sensors[sensor_data_element(offset)] & sensor_data_mask(offset);
}

void poll_until_sensor_triggered(int clock_server_tid, int track_state_controller_tid, unsigned int offset) {
  message reply;
  int16_t current_sensors[10], leading_edge[10];

  for (int i = 0; i < 10; i += 1) {
    current_sensors[i] = 0;
  }

  do {
    Assert(Delay(clock_server_tid, REFRESH_PERIOD) == 0);
    get_sensors(track_state_controller_tid, &reply);
    get_leading_edge(current_sensors, reply.msg.sensors, leading_edge);
  } while (!is_sensor_triggered(leading_edge, offset));
}
