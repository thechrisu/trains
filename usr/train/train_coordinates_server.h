/**
 * @file
 * @brief Server that tracks train location, velocity, and acceleration
 */
#ifndef TRAIN_COORDINATES_SERVER_H
#define TRAIN_COORDINATES_SERVER_H

#include "codes.h"
#include "messages.h"
#include "prediction.h"
#include "sensors/sensor_interpreter.h"

void update_coordinates_helper(int now_ticks,
                               turnout_state turnout_states[NUM_TURNOUTS],
                               coordinates *c);
void train_coordinates_server();

#endif /* TRAIN_COORDINATES_SERVER_H */
