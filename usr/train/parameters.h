/**
 * @file
 * @brief Parameters that can be changed at runtime with the `set` command
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "tstdlib.h"

#define PARAMETER_NAME_LENGTH 30

typedef enum set_parameter {
  SET_T1TRAIN,
  SET_SWITCH_PADDING,
  MAX_PARAMETER,
} set_parameter;

// Train to attribute sensors to in T1.
extern int t1train;

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

#endif /* PARAMETERS_H */
