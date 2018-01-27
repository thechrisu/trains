#include "benchmark.h"

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

void process_measurement(struct measurement *m, int16_t data1[NUM_MSG], int16_t data2[NUM_MSG], char name[32]) {
  zip_subtract(data1, data2, m->data);
  m->avg = get_avg(m->data);
  m->worst = get_worst(m->data);
  m->worst_index = get_worst_index(m->data);
  m->var = get_variance(m->data);
  memcpy(m->name, name, 32);
}

void print_measurement_array(struct measurement *mms) {
#ifndef TESTING
  for (int i = 0; i < NUM_TIMEPOINTS - 2; i++) {
    logprintf("%savg, %sworst, %svar, ", mms[i].name, mms[i].name, mms[i].name);
  }
  logprintf("%sAvg, %sWorst, %sVar\n\r",
           mms[NUM_TIMEPOINTS - 2].name,
           mms[NUM_TIMEPOINTS - 2].name,
           mms[NUM_TIMEPOINTS - 2].name);
  for (int i = 0; i < NUM_TIMEPOINTS - 2; i++) {
    logprintf("%d, %d, %d, ", mms[i].avg, mms[i].worst, mms[i].var);
  }
  logprintf("%d, %d, %d\n\r", mms[NUM_TIMEPOINTS - 2].avg, mms[NUM_TIMEPOINTS - 2].worst, mms[NUM_TIMEPOINTS - 2].var);
#else
  (void)mms;
#endif /* TESTING */
}
