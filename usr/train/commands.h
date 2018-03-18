/**
 * @file
 * @brief Functions for sending commands to the trains
 */
#ifndef TRAIN_COMMANDS_H
#define TRAIN_COMMANDS_H

#include "a0codes.h"
#include "codes.h"
#include "messages.h"
#include "tstdlib.h"
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
 * Stops a train, reverses it, and restarts it in the other direction at a given speed.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to reverse.
 * @param speed                      The speed to set the train to after reversing.
 */
void stop_and_reverse_train_to_speed(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int train, int speed);

/**
 * Stops a train, reverses it, and restarts it in the other direction at its previous speed.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param train_tx_server_tid        The task ID of the train transmit server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to reverse.
 */
void stop_and_reverse_train(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int train);

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
 * Queries the track state controller for train data.
 *
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param train                      The train to get data for.
 * @param tr_data                    A location in which to store the train data.
 */
void get_train(int track_state_controller_tid, int train, train_data *tr_data);

/**
 * Queries the track state controller for sensor data.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param reply                      A message in which to store the sensor data, coming from the track state controller's reply.
 */
void get_sensors(int track_state_controller_tid, message *reply);

/**
 * Queries the track state controller for turnout state.
 *
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param turnout_states             A location in which to store the turnout state.
 */
void get_turnouts(int track_state_controller_tid, turnout_state turnout_states[NUM_TURNOUTS]);

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
 * @param start                         Offset of the sensor at the start of the measurement.
 * @param end                           Offset of the sensor at the end of the measurement.
 * @param ticks                         Time it took to travel between the two sensors.
 */
void update_constant_velocity_model(int track_state_controller_tid, int train, int speed, unsigned int start, unsigned int end, uint32_t ticks);

/**
 * Gets the speed -> stopping distance mapping for a train in the track state controller.
 *
 * @param track_state_controller_tid Task ID of the track state controller.
 * @param train                      Train whose model to get.
 * @param reply                      reply.msg.train_distances[15] contains the stopping distances.
 */
void get_stopping_distance_model(int track_state_controller_tid, int train, message *reply);

/**
 * Updates the speed -> stopping distance mapping for a train in the track state controller.
 *
 * @param track_state_controller_tid    Task ID of the track state controller.
 * @param train                         Train whose model should be updated (1-80).
 * @param speed                         Speed mapping that should be adjusted (0-14).
 * @param distance                      Stopping distance to be set (In 100th mm).
 */
void update_stopping_distance_model(int track_state_controller_tid, int train, int speed, uint32_t distance);

/**
 * Gets the speed -> stopping time mapping for a train in the track state controller.
 *
 * @param track_state_controller_tid Task ID of the track state controller.
 * @param train                      Train whose model to get.
 * @param reply                      reply.msg.train_times[15] contains the stopping times.
 */
void get_stopping_time_model(int track_state_controller_tid, int train, message *reply);

/**
 * Updates the speed -> stopping time mapping for a train in the track state controller.
 *
 * @param track_state_controller_tid    Task ID of the track state controller.
 * @param train                         Train whose model should be updated (1-80).
 * @param speed                         Speed mapping that should be adjusted (0-14).
 * @param time                          Stopping time to be set (In microseconds).
 */
void update_stopping_time_model(int track_state_controller_tid, int train, int speed, uint32_t time);

/**
 * @param sensor_interpreter_tid Task ID of the sensor interpreter.
 * @param train                  Train number.
 * @param reply                  Location to put the reply in.
 */
void get_last_sensor_hit(int sensor_interpreter_tid, int train, reply_get_last_sensor_hit *last_sensor);

/**
 * @param train_coordinates_server Task ID of the train coordinates server.
 * @param train                    Train number.
 * @param c                        Location to put the train's coordinates in.
 */
void get_coordinates(int train_coordinates_server, int train, coordinates *c);

/**
 * Queries the acceleration model.
 * @param track_state_controller_tid Task ID of the track state controller.
 * @param train                      Train (1-80).
 * @return acceleration 1/100 m/(s^2).
 */
uint32_t get_acceleration(int track_state_controller_tid, int train);

/**
 * Update the acceleration model.
 * @param track_state_controller_tid Task ID of the track state controller.
 * @param train                      Train (1-80).
 * @param acc                        1/100 m/(s^2).
 */
void update_acceleration(int track_state_controller_tid, int train, uint32_t acc);
#endif /* TRAIN_COMMANDS_H */
