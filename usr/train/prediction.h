/**
 * @file
 * @brief Functions used for estimating a trains' current position from past data.
 */
#ifndef TRAIN_PREDICTION_H
#define TRAIN_PREDICTION_H

#include "codes.h"
#include "commands.h"
#include "global_track_state.h"
#include "messages.h"
#include "track_data.h"
#include "track_node.h"
#include "tstdlib.h"

#define INFINITE_TICKS 1E8

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
 * Predict the coordinates of a train the next time it is at a sensor.
 *
 * @param train_coordinates_server_tid Task ID of the train coordinates server.
 * @param turnout_states               State of the track's turnouts.
 * @param train                        Train to predict for.
 * @param prediction                   Place to store the prediction.
 */
void predict_sensor_hit(int train_coordinates_server_tid,
                        turnout_state turnout_states[NUM_TURNOUTS],
                        int train, coordinates *prediction);

#endif /* TRAIN_PREDICTION_H */
