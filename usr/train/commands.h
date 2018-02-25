/**
 * @file
 * @brief Functions for sending commands to the trains
 */
#ifndef TRAIN_COMMANDS_H
#define TRAIN_COMMANDS_H

#include "a0codes.h"
#include "codes.h"
#include "messages.h"
#include "track_data.h"

/**
 * Sets the speed of a train, both in reality and in the track state controller. The headlights are turned off.
 *
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to set the speed of.
 * @param speed                      The speed to accelerate or decelerate the train to.
 */
void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed);

/**
 * Sets the speed of a train and its headlights, both in reality and in the track state controller.
 *
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to set the speed of.
 * @param speed                      The speed to accelerate or decelerate the train to.
 * @param headlights                 Whether or not the headlights should be on.
 */
void set_train_speed_and_headlights(int train_tx_server_tid, int track_state_controller_tid, int train, int speed, bool headlights);

/**
 * Reverses a train, both in reality and in the track state controller.
 *
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to reverse.
 */
void reverse_train(int train_tx_server_tid, int track_state_controller_tid, int train);

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

/**
 * Queries the track state controller for sensor data.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param reply                      A message in which to store the sensor data, coming from the track state controller's reply.
 */
void get_sensors(int track_state_controller_tid, message *reply);

/**
 * Gets the speed -> velocity mapping for a train in the track state controller.
 *
 * @param track_state_controller_tid Task ID of the track state controller.
 * @param train                      Train whose model to get.
 * @param reply                      reply.msg.train_speeds[15] contains the velocities.
 */
void get_constant_velocity_model(int track_state_controller_tid, int train, message *reply);

/**
 * Updates the speed -> velocity mapping for a train in the track state controller.
 *
 * @param track_state_controller_tid    Task ID of the track state controller.
 * @param train                         Train whose model should be updated (1-80).
 * @param speed                         Speed mapping that should be adjusted (0-14).
 * @param velocity                      Velocity to be set (In 100th mm/s).
 */
void update_constant_velocity_model(int track_state_controller_tid, int train, int speed, uint32_t velocity);
#endif /* TRAIN_COMMANDS_H */
