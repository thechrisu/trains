/**
 * @file
 * @brief Functions to use the timers on the versatilepb platform
 */
#ifndef TRAINS_VERSATILEPB_TIMER_H
#define TRAINS_VERSATILEPB_TIMER_H


#include "stdlib.h"
#include "timer_data.h"
#include "versatilepb.h"

void setup_timer();
void setup_timer1(uint16_t countdown_val);
uint32_t get_time();
uint32_t get_clockticks();
void get_time_struct(my_time *sto, uint32_t *timestamp);

void interrupt_timer_setup();
void interrupt_timer_teardown();
void interrupt_timer_clear();

#endif //TRAINS_TIMER_H
