#include "kusage_stats.h"

usage_stats total_usage, usage_last_second;
uint32_t last_interval_start;
uint32_t ticks_this_second;
uint32_t last_called_my_frac;
bool started;

void reset_usage(usage_stats *stats) {
  for (int i = 0; i < MAX_TASKS; i++) {
    stats->ms_run[i] = 0;
  }
}

void setup_kusage_stats() {
  ticks_this_second = 0;
  last_called_my_frac = get_clockticks();
  reset_usage(&total_usage);
  reset_usage(&usage_last_second);
  started = false;
}

void start_interval() {
  kassert(!started);
  started = true;
  last_interval_start = get_clockticks();
  kassert(last_interval_start > 0);
}

void end_interval(int32_t tid) {
  kassert(started);
  started = false;
  kassert(last_interval_start > 0);
  uint32_t i = get_clockticks() - last_interval_start;
  total_usage.ms_run[tid] += i;
  usage_last_second.ms_run[tid] += i;
  //kassert(total_usage.ms_run[tid] < get_clockticks());
}

int32_t syscall_my_proc_usage(int32_t tid) {
  uint32_t total_last_second_time = get_clockticks() - last_called_my_frac;
  last_called_my_frac = get_clockticks();
  int32_t ret = 0.5 + (1000.0 * usage_last_second.ms_run[tid]) / total_last_second_time;
  reset_usage(&usage_last_second);
  return ret;
}

void syscall_total_proc_usage(usage_stats *t_usage) {
  for (int i = 0; i < MAX_TASKS; i++) {
    t_usage->ms_run[i] = total_usage.ms_run[i];
  }
  t_usage->max_tid = MAX_TASKS - 1;
}

void syscall_last_secs_proc_usage(usage_stats *t_usage) {
  for (int i = 0; i < MAX_TASKS; i++) {
    t_usage->ms_run[i] = usage_last_second.ms_run[i];
  }
  t_usage->max_tid = MAX_TASKS - 1;
  reset_usage(&usage_last_second);
}
