#include "timer.h"

volatile uint32_t timer3_countdown;
volatile uint32_t timer3_divisor;
volatile uint32_t timer3_lastval;
volatile uint64_t sys_time;

/**
 * Sets TC3 to operate in free-running mode
 * 0xFFFF FFFF takes x ms to run
 *
 * Note that we don't use free-running values.
 * Free-running mode can be simulated by setting countdown_val to 0xFFFF FFFF
 *
 * Note: Choosing 2kHz clock may result in us losing ticks
 * (see previous report on A0: Main loop takes 0.5ms -> 2kHz exactly)
 * Also note: All the functions currently expect you to use the fine-grained timer
 * In other words: YOUR PROGRAM WILL NOT WORK IF YOU CHOOSE CLOCK 0
 * @param countdown_val default is 0xFFFF FFFF
 * @param clock_to_use 1: 508kHz clock (recommended!), 0: 2kHz clock
 */
void set_tc3_timer(uint16_t countdown_val, bool clock_to_use) {
  timer3_countdown = countdown_val;
  timer3_lastval = countdown_val;
  sys_time = 0;
  timer3_divisor = (clock_to_use ? 5084689 : 2);
  volatile uint32_t *timer_ctrl = (uint32_t *) TIMER3_CTRL;
  volatile uint32_t *timer_load = (uint32_t *) TIMER3_LOAD;
  *timer_load = countdown_val;
  *timer_ctrl = (clock_to_use ? TIMER3_CLK : 0) | TIMER3_MODE;
  *timer_ctrl = *timer_ctrl | TIMER3_ENABLE;
}

/**
 * TC3 generates interrupt every time it reaches 0
 */
void clear_tc3_interrupt() {
  volatile uint32_t *timer_clr = (uint32_t *) TIMER3_CLEAR;
  *timer_clr = 1;
}

/**
 * Gets the time from TC3 (page 425 in Cirrus Logic manual, ch14)
 * TC3 generates an interrupt, so this may be useful for context-switching later
 * @param sto
 */
uint64_t get_tc3_timer_time() {
  volatile uint32_t register_contents = *((uint32_t *) TIMER3_VALUE);
  if (register_contents > timer3_lastval) { // overflow
    sys_time += (timer3_lastval + (timer3_countdown - register_contents));
  } else {
    sys_time += (timer3_lastval - register_contents);
  }
  timer3_lastval = register_contents;
  return sys_time;
}

/**
 * Sets timer 4 (debug timer)
 * @param enable whether to enable/disable the timer
 */
static void set_debug_timer(bool enable) {
  volatile uint8_t *timer_hi_onoff = (uint8_t *) TIMER4_HI_ONOFF;
  *timer_hi_onoff = enable ? TIMER4_ONOFF_MSK : 0;
}

/**
 * Does not generate interrupts!
 * @return time in millis
 */
static uint64_t get_debug_timer_time() {
  volatile uint64_t x = *((uint32_t *) TIMER4_VAL_LO);
  x += (uint64_t) (*((uint8_t *) TIMER4_HI_ONOFF) & TIMER4_HI_VAL_MSK) << 32;
  return (100 * x) / 98304; // MAGIC NUMBER see Cirrus manual

}

/**
 * Currently sets the debug timer.
 */
void setup_timer() {
  // set_debug_timer(true);
  set_tc3_timer(0xFFFF, 1);
  set_debug_timer(0);
  get_debug_timer_time();
}

/**
 * Wrapper function for hardware-abstraction :)
 * @return time in millis
 */
uint32_t get_time() {
  // return get_debug_timer_time();
  get_tc3_timer_time();
  return (10000*sys_time) / timer3_divisor;
}

/**
 * Gets time in ticks
 */
uint32_t get_clockticks() {
  get_tc3_timer_time();
  return sys_time;
}

void get_time_struct(my_time *sto, uint32_t *timestamp) {
  get_tc3_timer_time();
  uint32_t adjusted_time = (10000*sys_time) / timer3_divisor;
  *timestamp = adjusted_time;
  sto->min = adjusted_time / (60 * 1000);
  sto->sec = (adjusted_time / 1000) % 60;
  sto->dsec = (adjusted_time / 100) % 10;
}
