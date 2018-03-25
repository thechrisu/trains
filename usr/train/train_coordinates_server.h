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

void train_coordinates_server();

#endif /* TRAIN_COORDINATES_SERVER_H */
