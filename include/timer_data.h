/**
 * @file
 * @brief Timer data structures
 */
#ifndef TRAINS_TIMER_DATA_H
#define TRAINS_TIMER_DATA_H

#include "stdlib.h"

struct my_time {
  uint32_t min;
  uint32_t sec;
  uint32_t dsec;
};

typedef struct my_time my_time;

#endif //TRAINS_TIMER_DATA_H
