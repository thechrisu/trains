#ifndef TRAINS_BENCHMARK_H
#define TRAINS_BENCHMARK_H

#include "stdlib.h"

#define NUM_TIMEPOINTS (int)11
#define NUM_MSG 1000

enum benchmark_timepoints {
  BEFORE_SEND,
  AFTER_SEND,
  BEFORE_COPY,
  AFTER_COPY,
  BEFORE_SCHEDULE,
  AFTER_SCHEDULE,
  KENTRY_SYS_SEND,
  BEFORE_RECEIVE,
  KENTRY_SYS_RECEIVE,
  KEXIT_SYS_RECEIVE,
  AFTER_RECEIVE,
  KENTRY_SYS_REPLY,
  KEXIT_SYS_SEND
};

/*
volatile int16_t *loc_kEntry_sys_send = (int16_t *)      0x01a00000;
volatile int16_t *loc_before_receive = (int16_t *)       0x01a00004;
volatile int16_t *loc_kEntry_sys_receive = (int16_t *)   0x01a00008;
volatile int16_t *loc_kExit_sys_receive = (int16_t *)    0x01a0000C;
volatile int16_t *loc_after_receive = (int16_t *)        0x01a0010;
volatile int16_t *loc_kEntry_sys_reply = (int16_t *)     0x01a0014;
volatile int16_t *loc_kExit_sys_send = (int16_t *)       0x01a0018;
volatile int16_t *loc_before_copy = (int16_t *)          0x01a001C;
volatile int16_t *loc_after_copy = (int16_t *)           0x01a0020;
volatile int16_t *loc_before_schedule = (int16_t *)      0x01a0024;
volatile int16_t *loc_after_schedule = (int16_t *)       0x01a0028; // TODO put this in lib/benchmark_locations.c
*/

#endif /* TRAINS_BENCHMARK_H */
