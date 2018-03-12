/**
 * @file
 * @brief Task responsible for keeping position/routing of ONE train.
 */
#ifndef TRAIN_CONDUCTOR_H
#define TRAIN_CONDUCTOR_H

#include "codes.h"
#include "commands.h"
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

#endif /* TRAIN_CONDUCTOR_H */
