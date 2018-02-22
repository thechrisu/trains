/**
 * @file
 * @brief Functions for sending commands to the trains
 */
#ifndef TRAIN_COMMANDS_H
#define TRAIN_COMMANDS_H

#include "a0codes.h"
#include "codes.h"
#include "messages.h"

/**
 * Sets the speed of a train, both in reality and in the track state controller.
 *
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to set the speed of.
 * @param speed                      The speed to accelerate or decelerate the train to.
 */
void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed);

/**
 * Reverses a train, both in reality and in the track state controller.
 *
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to reverse.
 * @param speed                      The current direction of the train.
 */
void reverse_train(int train_tx_server_tid, int track_state_controller_tid, int train, bool current_direction);

/**
 * Switches a turnout, both in reality and in the track state controller.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param turnout_num                The turnout to switch.
 * @param curved                     Whether to make the turnout straight or curved.
 */
void switch_turnout(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int turnout_num, bool curved);

#endif /* TRAIN_COMMANDS_H */
