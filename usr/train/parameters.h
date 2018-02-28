/**
 * @file
 * @brief Parameters that can be changed at runtime with the `set` command
 */
#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "tstdlib.h"

typedef enum set_parameter {
  SET_T1TRAIN,
  MAX_PARAMETER,
} set_parameter;

extern int t1train;

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
