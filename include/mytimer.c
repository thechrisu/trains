//
// Created by Christoph Ulshöfer on 2018-01-08.
//

#include "mytimer.h"

#if VERSATILEPB


#else
#if TESTING
clock_t start_clock;
void setup_timer() {
  start_clock = clock();
}
uint32_t get_time() {
  return (uint32_t)(1000.0 * (clock() - start_clock) / CLOCKS_PER_SEC);
}
uint32_t get_cached_time() {
  return get_time();
}
#else // trainslab config

#endif
#endif
