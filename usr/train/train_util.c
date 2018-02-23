#include "train_util.h"

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]) {
  for (int i = 0; i  < 10; i++) {
    leading_edge[i] = ~old_sensors[i] & new_sensors[i]; // 0 -> 1
    old_sensors[i] = new_sensors[i];
  }
}

