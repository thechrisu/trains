/**
 * @file
 * @brief Functions to use the timers on the versatilepb platform
 */
#ifndef TRAINS_VERSATILEPB_TIMER_H
#define TRAINS_VERSATILEPB_TIMER_H


#include "../glue/timer_data.h"
#include "versatilepb.h"
#include "../../../src/stdlib.h"

void setup_timer();
void setup_timer1(uint16_t countdown_val);
uint32_t get_time();
uint32_t get_clockticks();
void get_time_struct(my_time *sto, uint32_t *timestamp);

#endif //TRAINS_TIMER_H
