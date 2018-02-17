/**
 * @file
 * @brief Functions for using the timers on the TS7200
 */
#ifndef TRAINS_LAB_TIMER_H
#define TRAINS_LAB_TIMER_H

#include "tstdlib.h"
#include "timer_data.h"
#include "ts7200.h"

void setup_timer();
uint32_t get_time();
uint32_t get_clockticks();
void get_time_struct(my_time *sto, uint32_t *timestamp);

void interrupt_timer_setup();
void interrupt_timer_teardown();
void interrupt_timer_clear();

#endif //TRAINS_LAB_TIMER_H
