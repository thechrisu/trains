/**
 * @file
 * @brief Definition of the train-speed-velocity triple.
 */
#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include "tstdlib.h"

typedef struct {
  int train;
  int speed;
  uint32_t velocity; // in 1/100th mm/s
} default_speed;

#endif /* MODEL_DATA_H */
