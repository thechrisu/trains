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

#define NO_DATA_RECEIVED 0xDEDEDEDE

void sensor_interpreter();
void send_attributed_sensor_to_coord_server(int coord_server, int train,
                                            unsigned int ls, unsigned int tls);

#endif /* SENSOR_INTERPRETER_H */
