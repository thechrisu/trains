/**
 * @file
 * @brief Definition of the train-speed-velocity triple.
 */
#ifndef MODEL_DATA_H
#define MODEL_DATA_H

#include "tstdlib.h"

 // velocity: in 1/100th mm/s
 // stopping distance in 1/100th mm
 // stopping time in microseconds
typedef struct {
  int train;
  int speed;
  uint32_t value;
} default_value;

#endif /* MODEL_DATA_H */
