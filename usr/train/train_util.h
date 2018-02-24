#ifndef TRAIN_UTIL_H
#define TRAIN_UTIL_H

#include "commands.h"
#include "terminal.h"
#include "tstdlib.h"

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]);

/**
 * @param   sensors A set of sensor data.
 * @param   offset  The offset of a sensor within the sensor data bitmap.
 * @returns Whether or not the sensor is triggered in the sensor data.
 */
bool is_sensor_triggered(int16_t sensors[10], unsigned int offset);

/**
 * Query the track state controller until the given sensor has been triggered.
 * @param clock_server_tid
 * @param track_state_controller_tid
 * @param offset                     The offset of a sensor within the sensor data bitmap.
 */
void poll_until_sensor_triggered(int clock_server_tid, int track_state_controller_tid, unsigned int offset);

#endif /* TRAIN_UTIL_H */

