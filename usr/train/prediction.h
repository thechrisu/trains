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
#include "train_util.h"
#include "tstdlib.h"

#define INFINITE_TICKS 1E8

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

void synthesize_turnout_states_from_route(track_node *route[MAX_ROUTE_LENGTH],
                                          turnout_state states[NUM_TURNOUTS]);

// TODO document this
void predict_train_stop(coordinates *c, track_node *route[MAX_ROUTE_LENGTH],
                        coordinates *send_stop_here, uint32_t train_speeds[15],
                        uint32_t train_distances[15]);
#endif /* TRAIN_PREDICTION_H */
