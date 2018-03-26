/**
 * @file
 * @brief Task responsible for keeping position/routing of ONE train.
 */
#ifndef TRAIN_CONDUCTOR_H
#define TRAIN_CONDUCTOR_H

#include "codes.h"
#include "commands.h"
#include "coordinate_courier.h"
#include "global_track_state.h"
#include "prediction.h"
#include "router.h"
#include "track_data.h"
#include "track_node.h"
#include "train_util.h"
#include "sensors/sensor_interpreter.h"

/**
 * The train conductor works by receiving messges from the command dispatcher,
 * indicating the next thing the train should do, e.g. route somewhere,
 * reverse the train, or just set its speed.
 */
void train_conductor();

/**
 * Sets the speed of the train.
 *
 * train_tx_server          Train Transmit Server.
 * track_state_controller   Task administering the global state.
 * train                    Train number
 * speed                    Speed to set train to.
 */
void conductor_setspeed(int train_tx_server, int track_state_controller, int train,
                        int speed);

/**
 * Reverses the train.
 *
 * train_tx_server          Train Transmit Server.
 * track_state_controller   Task administering the global state.
 * clock_serer              Clock server
 * train                    Train number
 */
void conductor_reverse(int train_tx_server, int track_state_controller, int clock_server,
                       int train);

/**
 * Create new triggers from the current state.
 *
 * @param c                    The current position.
 * @param train_speeds         Speed->Velocity map.
 * @param train_distances      Speed->Stopping distance map.
 * @param route                The route we're on.
 * @param locations_to_observe Output: New triggers are in this array.
 * @param n_requests           Output: The number of new triggers.
 */
void craft_new_triggers(coordinates *c, uint32_t train_speeds[15],
                        uint32_t train_distances[15],
                        track_node *route[MAX_ROUTE_LENGTH],
                        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
                        int *n_requests);

/**
 * Given a certain state, set the triggers for e.g. next switches or stopping.
 *
 * @param coord_courier                Tid of the coordinate courier.
 * @param c                            Our position at the moment.
 * @param route                        The route we're on.
 * @param train_speeds                 Speed->Velocity map.
 * @param train_distances              Speed->Stopping Distance map.
 * @param drop_existing_notificaitons  Whether the coordinator should drop
 *                                     existing notifications before adding the
 *                                     new triggers.
 */
void set_new_triggers(int coord_courier,
                      coordinates *c, track_node *route[MAX_ROUTE_LENGTH],
                      uint32_t train_speeds[15], uint32_t train_distances[15],
                      bool drop_existing_notifications);
#endif /* TRAIN_CONDUCTOR_H */
