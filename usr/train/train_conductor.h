/**
 * @file
 * @brief Task responsible for keeping position/routing of ONE train.
 */
#ifndef TRAIN_CONDUCTOR_H
#define TRAIN_CONDUCTOR_H

#include "codes.h"
#include "commands.h"
#include "track_node.h"
#include "train_util.h"
#include "track_state_controller.h"

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
 * clock_serer              Clock server
 * d                        Train data
 * speed                    Speed to set train to.
 */
void conductor_setspeed(int train_tx_server, int track_state_controller, int clock_server,
                        train_data *d, int speed);

/**
 * Reverses the train.
 *
 * train_tx_server          Train Transmit Server.
 * track_state_controller   Task administering the global state.
 * clock_serer              Clock server
 * d                        Train data
 */
void conductor_reverse(int train_tx_server, int track_state_controller, int clock_server,
                       train_data *d);

#endif /* TRAIN_CONDUCTOR_H */
