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
 * Watch out! This function will fail an assertion if start and end are the same.
 *
 * @param   t     The track to base the distance calculation off of.
 * @param   start The offset of the sensor to start at.
 * @param   end   The offset of the sensor to finish at.
 * @returns The distance between the two sensors.
 */
uint32_t distance_between_sensors(track_state *t, unsigned int start, unsigned int end);

#endif /* TRACK_DATA_H */
