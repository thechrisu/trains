//
// Created by Christoph Ulshöfer on 2018-01-05.
//

#ifndef TRAINS_TIMER_H
#define TRAINS_TIMER_H

#if TESTING
#else

#include "timer_data.h"
#include "../../src/stdlib.h"

void setup_timer();
uint64_t get_time();
uint64_t get_picky_time();
uint64_t get_cached_time();
void get_time_struct(my_time *sto, uint32_t *timestamp);
// void millis_to_time(uint32_t millis, time *time_sto);

#endif

#endif //TRAINS_TIMER_H
