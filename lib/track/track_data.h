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
 * Consider three sequential sensor pairs: A, B, and C. B1 and or B2 may be dead.
 * Given this setup, here are the cases this function considers.
 *
 * - A train hits A1, B1 (dead), and C1. A1 and C1 are triggered.
 * - A train hits A1, reverses, touches B1 (dead), then hits A2. A1 and A2 are triggered.
 *   (This is already covered by `sensors_are_paired`).
 * - A train hits A1, reverses, hits B1 (dead), hits B2, then hits A2. A1, B2, and A2 are triggered.
 * - A train hits A1, reverses, hits B1, hits B2 (dead), then hits A2. A1, B1, and A2 are triggered.
 * - A train hits A1, hits B1 (dead), reverses, touches C1, hits B2, then hits A2.
 *   A1, C1, B2 and A2 are triggered. A1 -> C1 and C1 -> B2 are both handled by this function.
 * - A train hits A1, hits B1, reverses, touches C1, hits B2 (dead), then hits A2.
 *   A1, B1, C1, and A2 are triggered. C1 -> A2 is handled by this function.
 * - A train hits A1, hits B1, reverses, hits C1, C2, B2, and then A2. This function handles the
 *   cases where B1 and/or B2 are dead.
 *
 * @param   t      A track state.
 * @param   start  The start sensor.
 * @param   end    The end sensor.
 * @returns Whether or not the end sensor might be triggered after the start if there is a dead sensor.
 */
bool sensor_may_be_seen_after(track_state *t, unsigned int start, unsigned int end);

#endif /* TRACK_DATA_H */
