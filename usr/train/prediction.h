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
#include "train_coordinates_server.h"
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

/**
 * Given a route, record which direction the turnouts should be set to.
 *
 * @param route                   Route.
 * @param states                  Array that will contain correct values
 *                                for all turnouts used in the route.
 *                                For turnouts not used in the route,
 *                                no guarantee is made w.r.t. states
 */
void synthesize_turnout_states_from_route(track_node *route[MAX_ROUTE_LENGTH],
                                          turnout_state states[NUM_TURNOUTS]);

/**
 * Given a track, predict the location where we have to stop.
 *
 * @param c                        Current coordinates.
 * @param route                    The route we're currently on.
 * @param send_stop_here           Output coordinates, will contain target
 *                                 location.
 * @param train_speeds             Speed-to-velocity array.
 * @param train_distances          Stopping distance array for the given train.
 */
void predict_train_stop(coordinates *c, track_node *route[MAX_ROUTE_LENGTH],
                        coordinates *send_stop_here, uint32_t train_speeds[15],
                        uint32_t train_distances[15]);

void predict_next_switch(coordinates *co, track_node *route[MAX_ROUTE_LENGTH],
                        coordinates *send_switch_here, int *next_turnout_num,
                        bool *next_is_curved, bool *found, int distance,
                        int max_lookahead);

bool will_collide_with_other_train(int distance, coordinates *c, coordinates others[],
                                   int num_other_trains, turnout_state turnout_states[NUM_TURNOUTS],
                                   int *highest_acceptable_speed, int train);
#endif /* TRAIN_PREDICTION_H */
