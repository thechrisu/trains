//
// Created by Christoph Ulshöfer on 2018-01-08.
//

#include "mytimer.h"

#if TESTING
clock_t start_clock;
void setup_timer() {
  start_clock = clock();
}
uint64_t get_time() {
  return (uint64_t)(1000.0 * (clock() - start_clock) / CLOCKS_PER_SEC);
}
uint64_t get_cached_time() {
  return get_time();
}
#endif
