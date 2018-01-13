//
// Created by Christoph Ulshöfer on 2018-01-05.
//

#ifndef TRAINS_LAB_TIMER_H
#define TRAINS_LAB_TIMER_H

#include "../glue/timer_data.h"
#include "ts7200.h"
#include "../../../src/stdlib.h"

void setup_timer();
uint32_t get_time();
uint32_t get_clockticks();
void get_time_struct(my_time *sto, uint32_t *timestamp);

#endif //TRAINS_LAB_TIMER_H
