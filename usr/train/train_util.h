#ifndef TRAIN_UTIL_H
#define TRAIN_UTIL_H

#include "commands.h"
#include "terminal.h"
#include "tstdlib.h"

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]);

bool is_sensor_triggered(int16_t sensors[10], char bank, unsigned int index);

void poll_until_sensor_triggered(int clock_server_tid, int track_state_controller_tid, char bank, int index);

#endif /* TRAIN_UTIL_H */

