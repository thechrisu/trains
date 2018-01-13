//
// Created by Christoph Ulshöfer on 2018-01-12.
//

#include "timer.h"

volatile uint32_t timer1_countdown;
volatile uint32_t timer1_divisor;
volatile uint32_t timer1_lastval;
volatile uint64_t sys_time;

void setup_timer() {
  setup_timer1(0xFFFFFFFF);
}

void setup_timer1(uint16_t countdown_val) {
  volatile unsigned int *TIMER_VALUE = (unsigned int *)(TIMER1_BASE + LDR_OFFSET);
  volatile unsigned int *TIMER_CTRL = (unsigned int *)(TIMER1_BASE + CTRL_OFFSET);
  sys_time = 0;
  timer1_divisor = 1000000; // TODO find more fine-grained value
  timer1_countdown = countdown_val;
  timer1_lastval = countdown_val;
  *TIMER_VALUE = countdown_val;
  *TIMER_CTRL = (ENABLE_MASK | TIMER_SIZE) & ~ENABLE_INTERRUPT;
}

void clear_interrupt() {
  volatile unsigned int *INTERRUPT_CLEAR = (unsigned int *)(TIMER1_BASE + CLR_OFFSET);
  *INTERRUPT_CLEAR = 1;
}

uint32_t get_time() {
  volatile uint32_t register_contents = *((uint32_t *)(TIMER1_BASE + VAL_OFFSET));
  /*if (nloops == 16*16*16 - 1) {
    register_contents = (*((uint32_t *) TIMER1_BASE + VAL_OFFSET)) >> 16;
    __asm__("LDR pc, =0xfffffff");
  }*/
  if (register_contents > timer1_lastval) { // overflow
    sys_time += (timer1_lastval + (timer1_countdown - register_contents));
  } else {
    sys_time += (timer1_lastval - register_contents);
  }
  timer1_lastval = register_contents;
  return (1000 * sys_time) / timer1_divisor;
}

uint32_t get_clockticks() {
  get_time();
  return sys_time;
}

// TODO write main function in glue, only change get_time, setup
void get_time_struct(my_time *sto, uint32_t *timestamp) {
  get_time();
  uint32_t adjusted_time = (1000 * sys_time) / timer1_divisor;
  *timestamp = adjusted_time;
  sto->min = adjusted_time / (60 * 1000);
  sto->sec = (adjusted_time / 1000) % 60;
  sto->dsec = (adjusted_time / 100) % 10;
}
