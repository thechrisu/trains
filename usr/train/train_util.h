#ifndef TRAIN_UTIL_H
#define TRAIN_UTIL_H

#include "commands.h"
#include "terminal.h"
#include "tstdlib.h"

/**
 * @param old_sensors  The previous sensor data.
 * @param new_sensors  The current sensor data.
 * @param leading_edge A buffer in which to store information about which sensors were triggered this sensor read.
 */
void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]);

/**
 * @param   sensors A set of sensor data.
 * @param   offset  The offset of a sensor within the sensor data bitmap.
 * @returns Whether or not the sensor is triggered in the sensor data.
 */
bool is_sensor_triggered(int16_t sensors[10], unsigned int offset);

/**
 * Poll the track state controller for sensor data until either
 * 1) the given sensor has been triggered or
 * 2) the number of ticks spent polling exceed the timeout.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param offset                     The offset of a sensor within the sensor data bitmap.
 * @param timeout_ticks              Ticks until we return.
 * @return                           1 if it timed out, 0 if the sensor was received.
 */
int poll_until_sensor_triggered_with_timeout(int clock_server_tid,
                                             int track_state_controller_tid,
                                             unsigned int offset,
                                             int timeout_ticks);

/**
 * Poll the track state controller for sensor data until the given sensor has been triggered.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param offset                     The offset of a sensor within the sensor data bitmap.
 */
void poll_until_sensor_triggered(int clock_server_tid, int track_state_controller_tid, unsigned int offset);

#endif /* TRAIN_UTIL_H */
