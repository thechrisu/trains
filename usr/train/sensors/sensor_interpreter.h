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

/**
 * Sensor interpreter task.
 */
void sensor_interpreter();

/**
 * Wrapper for telling the train coordinate server that a certain train is
 * at a certain location.
 *
 * @param coord_server                     Tid of the train coordinate server.
 * @param train                            1-80.
 * @param ls                               The offset of the last seen sensor.
 * @param tls                              Ticks at which we've last seen the
 *                                         sensor.
 */
void send_attributed_sensor_to_coord_server(int coord_server, int train,
                                            unsigned int ls, unsigned int tls);

#endif /* SENSOR_INTERPRETER_H */
