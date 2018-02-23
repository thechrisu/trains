#ifndef TRAIN_MODEL_H
#define TRAIN_MODEL_H

#include "codes.h"
#include "tstdlib.h"

typedef struct {
  int train;
  int speed;
  uint32_t velocity; // in 1/100th mm/s
} default_speed;

#define DEFINITE_MAX_CM_PER_SEC 60

extern default_speed default_speeds[];
extern uint32_t super_default_speeds[15];

void setup_speed_to_velocity_map(uint32_t map[81][15], default_speed defaults[], uint32_t super_defaults[15]);

#endif /* TRAIN_MODEL_H */
