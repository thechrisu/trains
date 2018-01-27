/**
 * @file
 * @brief Defines encodings used for measuring the messaging/context switch performance.
 */
#ifndef TRAINS_BENCHMARK_H
#define TRAINS_BENCHMARK_H

#include "assert.h"
#include "myio.h"
#include "tstdlib.h"

#define NUM_TIMEPOINTS (int)11
#define NUM_MSG 2

#define LOC_KENTRY_SYS_SEND     (int16_t *) 0x01A00000;
#define LOC_BEFORE_RECEIVE      (int16_t *) 0x01A00004;
#define LOC_KENTRY_SYS_RECEIVE  (int16_t *) 0x01A00008;
#define LOC_KEXIT_SYS_RECEIVE   (int16_t *) 0x01A0000C;
#define LOC_AFTER_RECEIVE       (int16_t *) 0x01A00010;
#define LOC_KENTRY_SYS_REPLY    (int16_t *) 0x01A00014;
#define LOC_KEXIT_SYS_SEND      (int16_t *) 0x01A00018;
#define LOC_BEFORE_COPY         (int16_t *) 0x01A0001C;
#define LOC_AFTER_COPY          (int16_t *) 0x01A00020;
#define LOC_BEFORE_SCHEDULE     (int16_t *) 0x01A00024;
#define LOC_AFTER_SCHEDULE      (int16_t *) 0x01A00028;
#define TID_SEND                (int16_t *) 0x01A000A0;
#define TID_RECEIVE_REPLY       (int16_t *) 0x01A000A4;
#define IS_RECEIVE              (int16_t *) 0x01A000A8;

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

/**
 * Given an array of messages (int), returns the average value.
 * @param a Integer array (e.g. of measurement values)
 * @return average of the NUM_MSG values.
 */
int16_t get_avg(int16_t a[NUM_MSG]);

/**
 * Returns the largest numer.
 * @param a Run time (thus largest -> worst).
 * @return The worst run time.
 */
int16_t get_worst(int16_t a[NUM_MSG]);

/**
 * Returns the index of the item in the given array with the largest value.
 * @param a Array.
 * @return Index in the input array with the largest value.
 */
int16_t get_worst_index(int16_t a[NUM_MSG]);

/**
 * Returns the variance of an array of measurement values.
 * @param a Array of run times.
 * @return Variance of the input array
 */
int16_t get_variance(int16_t a[NUM_MSG]);

/**
 * Applies a subtract operation to every element the first two arguments and saves the result in the third.
 * @param arg1 First argument.
 * @param arg2 Second argument.
 * @param res Result (equivalent to \forall arg1[i], arg2[i], res[i] = arg2[i] - arg1[i])
 */
void zip_subtract(int16_t arg1[NUM_MSG], int16_t arg2[NUM_MSG], int16_t res[NUM_MSG]);

/**
 * Populates the measurement data structure with the given data.
 * A measurement is defined to be an interval [data1, data2]
 * @param m Measurement pointer.
 * @param data1 Array of earlier measurement values.
 * @param data2 Array of later measurement values.
 * @param name Name of the measurement
 */
void process_measurement(struct measurement *m, int16_t data1[NUM_MSG], int16_t data2[NUM_MSG], char name[32]);

/**
 * Prints an array of measurements.
 * @param mms Array of size NUM_TIMEPOINTS - 2
 */
void print_measurement_array(struct measurement *mms);
#endif /* TRAINS_BENCHMARK_H */
