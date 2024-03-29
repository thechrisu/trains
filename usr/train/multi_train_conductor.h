/**
 * @file
 * @brief Task responsible for keeping position/routing of MULTIPLE trains.
 */
#ifndef MULTI_TRAIN_CONDUCTOR_H
#define MULTI_TRAIN_CONDUCTOR_H

#include "codes.h"
#include "commands.h"
#include "multi_coordinate_courier.h"
#include "global_track_state.h"
#include "prediction.h"
#include "router.h"
#include "sleeper.h"
#include "track_data.h"
#include "track_node.h"
#include "train_util.h"
#include "sensors/sensor_interpreter.h"

/**
 * The train conductor works by receiving messges from the command dispatcher,
 * indicating the next thing the train should do, e.g. route somewhere,
 * reverse the train, or just set its speed.
 */
void multi_train_conductor();

/**
 * Sets the speed of the lead train in a group.
 *
 * train_tx_server          Train Transmit Server.
 * track_state_controller   Task administering the global state.
 * group                    Train group
 * speed                    Speed to set lead train to.
 */
void multi_conductor_setspeed(int train_tx_server, int track_state_controller,
                              train_group *group, int speed);

/**
 * Reverses the train.
 *
 * train_tx_server          Train Transmit Server.
 * track_state_controller   Task administering the global state.
 * clock_serer              Clock server
 * group                    Train group
 */
void multi_conductor_reverse(int train_tx_server, int track_state_controller,
                       int clock_server, train_group *group);

#endif /* MULTI_TRAIN_CONDUCTOR_H */
