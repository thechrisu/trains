/**
 * @file
 * @brief Attributes sensor data to the correct train and runs constant velocity calibration
 */
#ifndef SENSOR_INTERPRETER_H
#define SENSOR_INTERPRETER_H

#include "codes.h"
#include "commands.h"
#include "messages.h"
#include "parameters.h"
#include "terminal.h"
#include "track_data.h"
#include "global_track_state.h"
#include "train_util.h"
#include "tstdlib.h"

void sensor_interpreter();

#endif /* SENSOR_INTERPRETER_H */
