/**
 * @file
 * @brief Definition of the train model.
 */
#ifndef TRAIN_MODEL_H
#define TRAIN_MODEL_H

#include "codes.h"
#include "model_data.h"
#include "tstdlib.h"

#define DEFINITE_MAX_CM_PER_SEC 70

/**
 * Initializes the speed->velocity map passed by the first argument.
 *
 * @param map            Map from train speeds to velocities.
 * @param defaults       An array of default values (special guard element defined in default_speeds) for each train.
 * @param super_defaults If a certain train has no default for a speed, this array defines the global default values.
 */
void setup_speed_to_velocity_map(uint32_t map[81][15], default_speed defaults[], uint32_t super_defaults[15]);

#endif /* TRAIN_MODEL_H */
