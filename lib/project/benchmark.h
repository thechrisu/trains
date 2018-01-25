/**
 * @file
 * @brief Defines encodings used for measuring the messaging/context switch performance.
 */
#ifndef TRAINS_BENCHMARK_H
#define TRAINS_BENCHMARK_H

#include "assert.h"
#include "myio.h"
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

struct measurement {
  int16_t data[NUM_MSG];
  char name[32];
  int16_t avg, worst, worst_index, var;
};

extern volatile int16_t *loc_kEntry_sys_send;
extern volatile int16_t *loc_before_receive;
extern volatile int16_t *loc_kEntry_sys_receive;
extern volatile int16_t *loc_kExit_sys_receive;
extern volatile int16_t *loc_after_receive;
extern volatile int16_t *loc_kEntry_sys_reply;
extern volatile int16_t *loc_kExit_sys_send;
extern volatile int16_t *loc_before_copy;
extern volatile int16_t *loc_after_copy;
extern volatile int16_t *loc_before_schedule;
extern volatile int16_t *loc_after_schedule;
extern volatile int *tid_send;
extern volatile int *tid_receive_reply;
extern volatile int *is_receive;

int16_t get_avg(int16_t a[NUM_MSG]);

int16_t get_worst(int16_t a[NUM_MSG]);

int16_t get_worst_index(int16_t a[NUM_MSG]);

int16_t get_variance(int16_t a[NUM_MSG]);

/**
 * Applies a subtract operation to every element the first two arguments and saves the result in the third.
 * @param arg1 First argument.
 * @param arg2 Second argument.
 * @param res Result (equivalent to \forall arg1[i], arg2[i], res[i] = arg2[i] - arg1[i])
 */
void zip_subtract(int16_t arg1[NUM_MSG], int16_t arg2[NUM_MSG], int16_t res[NUM_MSG]);

void process_measurement(struct measurement *m, int16_t data1[NUM_MSG], int16_t data2[NUM_MSG]);

void print_measurement_array(struct measurement *mms);
#endif /* TRAINS_BENCHMARK_H */
