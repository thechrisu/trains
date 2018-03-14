/**
 * @file
 * @brief Functions used for estimating a trains' current position from past data.
 */
#ifndef TRAIN_PREDICTION_H
#define TRAIN_PREDICTION_H

#include "codes.h"
#include "global_track_state.h"
#include "messages.h"
#include "track_data.h"
#include "track_node.h"
#include "tstdlib.h"

/**
 * Returns the distance we travelled since we last hit a sensor.
 *
 * @param clock_server          Tid of the clock server.
 * @param ticks_at_last_sensor  The time in ticks at the point when we last hit a sensor.
 * @param velocity              The train's velocity (in 1/100th mm/s).
 *
 * @return The distance in 1/100th mm that we have travelled.
 */
int dist_from_last_sensor(int clock_server, int ticks_at_last_sensor,
                          uint32_t velocity);

/**
 * Wrapper for <code>dist_From_last_sensor</code> that uses structs for
 * the last sensor record + the current offset from the last sensor.
 * Updates the offset in <code>current</code> according to the distance
 * travelled since hitting the last sensor.
 *
 * @param clock_server           Tid of the clock server.
 * @param velocity               The train's velocity (in 1/100th mm/s).
 * @param last_record            The last sensor we knew we hit.
 * @param current                'Current' location (offset maybe stale).
 */
void get_location_from_last_sensor_hit(int clock_server, int velocity,
                                       reply_get_last_sensor_hit *last_record,
                                       location *current);

void update_coordinates_after_sensor_hit(reply_get_last_sensor_hit *last_sensor_hit,
                                         turnout_state turnout_states[NUM_TURNOUTS],
                                         coordinates *c);

void update_coordinates_after_speed_change(train_data *tr_data,
                                           uint32_t velocity_model[15],
                                           int acceleration,
                                           turnout_state turnout_states[NUM_TURNOUTS],
                                           coordinates *c);

void update_coordinates_after_reverse(coordinates *c);

void update_coordinates_after_time_passed(int clock_server,
                                          turnout_state turnout_states[NUM_TURNOUTS],
                                          coordinates *c);

#endif /* TRAIN_PREDICTION_H */
