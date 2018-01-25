#include "benchmark.h"

volatile int16_t *loc_kEntry_sys_send = (int16_t *) 0x01a00000;
volatile int16_t *loc_before_receive = (int16_t *) 0x01a00004;
volatile int16_t *loc_kEntry_sys_receive = (int16_t *) 0x01a00008;
volatile int16_t *loc_kExit_sys_receive = (int16_t *) 0x01a0000C;
volatile int16_t *loc_after_receive = (int16_t *) 0x01a0010;
volatile int16_t *loc_kEntry_sys_reply = (int16_t *) 0x01a0014;
volatile int16_t *loc_kExit_sys_send = (int16_t *) 0x01a0018;
volatile int16_t *loc_before_copy = (int16_t *) 0x01a001C;
volatile int16_t *loc_after_copy = (int16_t *) 0x01a0020;
volatile int16_t *loc_before_schedule = (int16_t *) 0x01a0024;
volatile int16_t *loc_after_schedule = (int16_t *) 0x01a0028; // TODO put this in lib/benchmark_locations.c
volatile int* tid_send = (int *) 0x01a000A0;
volatile int* tid_receive_reply = (int *) 0x01a000A4;
volatile int* is_receive = (int *) 0x01a000A8;

int16_t get_avg(int16_t a[NUM_MSG]) {
  int32_t acc = 0;
  for (int i = 0; i < NUM_MSG; i++) {
    acc += a[i];
  }
  return (int16_t)(acc / NUM_MSG);
}

int16_t get_worst(int16_t a[NUM_MSG]) {
  int16_t worst = 0;
  for (int i = 0; i < NUM_MSG; i++) {
    if (worst < a[i]) worst = a[i];
  }
  return worst;
}

int16_t get_worst_index(int16_t a[NUM_MSG]) {
  int16_t worst = 0;
  int16_t worst_index = 0;
  for (int i = 0; i < NUM_MSG; i++) {
    if (worst < a[i]) {
      worst = a[i];
      worst_index = i;
    }
  }
  return worst_index;
}

int16_t get_variance(int16_t a[NUM_MSG]) {
  int32_t acc = 0;
  int16_t avg = get_avg(a);
  for (int i = 0; i < NUM_MSG; i++) {
    acc += (a[i] - avg) * (a[i] - avg);
  }
  return (int16_t)(acc / (NUM_MSG - 1.0));
}

void zip_subtract(int16_t arg1[NUM_MSG], int16_t arg2[NUM_MSG], int16_t res[NUM_MSG]) {
  for (int16_t i = 0; i < NUM_MSG; i++) {
    res[i] = arg2[i] - arg1[i];
    if (res[i] < 0)
      res[i] *= -1;
  }
}

void process_measurement(struct measurement *m, int16_t data1[NUM_MSG], int16_t data2[NUM_MSG]) {
  zip_subtract(data1, data2, m->data);
  m->avg = get_avg(m->data);
  m->worst = get_worst(m->data);
  m->worst_index = get_worst_index(m->data);
  m->var = get_variance(m->data);
}

void print_measurement_array(struct measurement *mms) {
  for (int i = 0; i < NUM_TIMEPOINTS - 2; i++) {
    bwprintf("%savg, %sworst, %svar, ", mms[i].name, mms[i].name, mms[i].name);
  }
  bwprintf("%sAvg, %sWorst, %sVar\n\r",
           mms[NUM_TIMEPOINTS - 2].name,
           mms[NUM_TIMEPOINTS - 2].name,
           mms[NUM_TIMEPOINTS - 2].name);
  for (int i = 0; i < NUM_TIMEPOINTS - 2; i++) {
    bwprintf("%d, %d, %d, ", mms[i].avg, mms[i].worst, mms[i].var);
  }
  bwprintf("%d, %d, %d\n\r", mms[NUM_TIMEPOINTS - 2].avg, mms[NUM_TIMEPOINTS - 2].worst, mms[NUM_TIMEPOINTS - 2].var);
}
