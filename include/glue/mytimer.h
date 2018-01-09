//
// Created by Christoph Ulshöfer on 2018-01-05.
//

#ifndef TRAINS_MYTIMER_H
#define TRAINS_MYTIMER_H

#include "../labenv/timer_data.h"

#if (HOSTCONFIG || TESTING)
#include <time.h>
void setup_timer();
uint64_t get_time();
uint64_t get_cached_time();
/*void get_time_struct(time *sto) {
  sto->min =
  sto->sec =
  sto->dsec =
  return
}*/
#else
#include "../labenv/timer.h"
#endif

#endif //TRAINS_MYTIMER_H
