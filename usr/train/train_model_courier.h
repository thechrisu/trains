/**
 * @file
 * @brief Train Model Courier.
 */
#ifndef TRAIN_MODEL_COURIER_H
#define TRAIN_MODEL_COURIER_H

#include "codes.h"
#include "messages.h"
#include "parameters.h"

/**
 * Periodically asks the track state controller to inform the
 * coordinates server about the latest train speeds of the active trains.
 */
void train_model_courier();

#endif /* TRAIN_MODEL_COURIER_H */
