/**
 * @file
 * @brief Helper functions for track/train state
 */
#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include "model.h"
#include "track_node.h"
#include "tstdlib.h"

void init_track(track_state *global_track);

unsigned int turnout_num_to_map_offset(unsigned int turnout);

unsigned int map_offset_to_turnout(unsigned int offset);

/**
 * Returns true if a turnout number is between 1 and 18 or between 153 and 156.
 *
 * @param   turnout A turnout number.
 * @returns Whether the turnout number is valid.
 */
bool is_valid_turnout_num(unsigned int turnout);

/**
 * @param   offset The index of a sensor within the sensor data bitmap.
 * @returns The letter of a bank (A to E) to which the sensor belongs.
 */
char sensor_bank(unsigned int offset);

/**
 * @param   offset The index of a sensor within the sensor data bitmap.
 * @returns The index of the sensor within its bank (1 to 16).
 */
unsigned int sensor_index(unsigned int offset);

/**
 * @param   offset The index of a sensor within the sensor data bitmap.
 * @returns The element of a sensor data array `sensor_data` in which the state of the sensor is found.
 */
unsigned int sensor_data_element(unsigned int offset);

/**
 * @param   offset The index of a sensor within the sensor data bitmap.
 * @returns The mask to apply to `sensor_data[sensor_data_element(offset)]` to get the sensor's state.
 */
unsigned int sensor_data_mask(unsigned int offset);

/**
 * @param   bank  The letter of the bank (A to E) to which the sensor belongs.
 * @param   index The index of the sensor within its bank.
 * @returns The index of the sensor within the sensor data bitmap.
 */
unsigned int sensor_offset(char bank, unsigned int index);

track_node *find_sensor(track_state *t, unsigned int offset);

/**
 * Returns the track node element of a sensor, given a track.
 *
 * @param t       Track state object.
 * @param offset  Offset of the sensor in the track.
 * @return The trackstate pointer to the sensor.
 */
track_node *find_sensor(track_state *t, unsigned int offset);

/**
 * Returns the distance between two sequential sensors -- aborts after exceeding limit.
 * The implementation of distance_between_sensors.
 *
 * @param   start             The track node of the sensor to start at.
 * @param   end               The track node of the sensor to finish at.
 * @param   total_distance    Accumulated parameter of the total distance so far.
 * @param   limit             The maximum depth to use to find a sensor.
 * @returns The distance between the two sensors.
 */
uint32_t distance_between_sensors_helper(track_node *start, track_node *end,
                                         uint32_t total_distance,
                                         int limit);

/**
 * Returns the distance between two sequential sensors.
 *
 * @param   t     The track to base the distance calculation off of.
 * @param   start The offset of the sensor to start at.
 * @param   end   The offset of the sensor to finish at.
 * @returns The distance between the two sensors.
 */
uint32_t distance_between_sensors(track_state *t, unsigned int start, unsigned int end);

/**
 * @param   t     The track to base the distance calculation off of.
 * @param   start The offset of the sensor to start at.
 * @param   end   The offset of the sensor to finish at.
 * @returns Whether start is directly followed by end.
 */
bool sensor_is_followed_by(track_state *t, unsigned int start, unsigned int end);

/**
 * @param   t      A track state.
 * @param   first  The first sensor.
 * @param   second The second sensor.
 * @returns Whether or not the two sensors are the two directions of one physical sensor.
 */
bool sensors_are_paired(track_state *t, unsigned int first, unsigned int second);

/**
 * Checks whether `end` could be triggered right after `start`, given that the train
 * might reverse once during the trip and there might be one dead sensor along the way.
 *
 * @param   t      A track state.
 * @param   start  The start sensor.
 * @param   end    The end sensor.
 * @returns Whether or not the end sensor may be seen after the start one.
 */
bool sensor_may_be_seen_next(track_state *t, unsigned int start, unsigned int end);

/**
 * Checks whether it's possible to reach `end` from `start` without reversing.
 *
 * @param   t      A track state.
 * @param   start  The start sensor.
 * @param   end    The end sensor.
 * @returns Whether or not the end sensor is reachable from the start one.
 */
bool sensor_reachable(track_state *t, unsigned int start, unsigned int end);

/**
 * Stores the reverse of a location in the given location.
 *
 * The reverse of a location is based on the pair of the location's sensor, with an
 * offset equal to the negative of the location's offset. A location and its reverse
 * are in the same real-world position on the track.
 * @param t           A track state.
 * @param destination The location to copy the reverse into.
 * @param source      The location to base the reverse off of.
 */
void location_reverse(track_state *t, location *destination, location *source);

#endif /* TRACK_DATA_H */
