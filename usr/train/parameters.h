/**
 * @file
 * @brief Parameters that can be changed at runtime with the `set` command
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "track_data.h"
#include "tstdlib.h"

#define NO_NEXT_SENSOR_EXPECTED 1234

#define PARAMETER_NAME_LENGTH 30

typedef enum set_parameter {
  MIN_PARAMETER,
  SET_TRAINS,
  SET_SWITCH_PADDING,
  SET_TRACK,
  MAX_PARAMETER,
} set_parameter;

// Trains that are active.
extern int active_trains[81];

// Number of trains active.
extern int num_active_trains;

// Expected next sensor for each train.
unsigned int expected_next_sensors[81];

// Minimum distance in mm that a turnout must be switched before a train drives over it.
extern int switch_padding;

/**
 * Initializes parameters to their default values.
 */
void init_parameters();

/**
 * @param   index The value in the set_parameter enum corresponding to the parameter.
 * @returns A pointer to a string containing the name of the parameter.
 */
char *get_parameter_name(set_parameter index);

/**
 * @param   name The name of a parameter.
 * @returns The value in the set_parameter enum corresponding to the name, or
 *          MAX_PARAMETER if no parameter matches.
 */
set_parameter get_parameter_value(char *name);

#endif /* PARAMETERS_H */
