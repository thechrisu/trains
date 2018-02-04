#include "kusage_stats.h"

#define MAX_POSSIBLE_CTX_SW_PER_S  100000

usage_stats total_usage;
int32_t_buffer ls_tids_buffer, ls_vals_buffer;
uint32_t ls_tids[MAX_POSSIBLE_CTX_SW_PER_S], ls_vals[MAX_POSSIBLE_CTX_SW_PER_S];
uint32_t last_interval_start;
uint32_t ticks_this_second;
uint32_t last_called_my_frac;
bool started;

void setup_kusage_stats() {
  int32_t_buffer_init(&ls_tids_buffer, (int32_t *)ls_tids, MAX_POSSIBLE_CTX_SW_PER_S);
  int32_t_buffer_init(&ls_vals_buffer, (int32_t *)ls_vals, MAX_POSSIBLE_CTX_SW_PER_S);
  ticks_this_second = 0;
  last_called_my_frac = get_clockticks();
  for (int i = 0; i < MAX_TASKS; i++) {
    total_usage.ms_run[i] = 0;
  }
  started = false;
}

void maybe_empty_last_secs_buffer() {
  ticks_this_second++;
  if (ticks_this_second >= 100) {
    int32_t_buffer_empty(&ls_tids_buffer);
    int32_t_buffer_empty(&ls_vals_buffer);
    ticks_this_second = 0;
  }
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
  kassert(total_usage.ms_run[tid] < get_clockticks());
  int32_t_buffer_put_replace(&ls_tids_buffer, tid);
  int32_t_buffer_put_replace(&ls_vals_buffer, i);
}

int32_t syscall_my_proc_usage(int32_t tid) {
  uint32_t my_runtime_last_second = 0;
  uint32_t total_last_second_time = get_clockticks() - last_called_my_frac;
  last_called_my_frac = get_clockticks();
  while (!int32_t_buffer_is_empty(&ls_tids_buffer)) {
    uint32_t buf_val = int32_t_buffer_get(&ls_vals_buffer);
    if (int32_t_buffer_get(&ls_tids_buffer) == tid) {
      my_runtime_last_second += buf_val;
    }
  }
  return (100 * my_runtime_last_second) / total_last_second_time;
}

void syscall_total_proc_usage(usage_stats *t_usage) {
  for (int i = 0; i < MAX_TASKS; i++) {
    t_usage->ms_run[i] = total_usage.ms_run[i];
  }
  t_usage->max_tid = next_task_id - 1;
}

void syscall_last_secs_proc_usage(usage_stats *t_usage) {
  for (int i = 0; i < next_task_id; i++) {
    t_usage->ms_run[i] = 0;
  }
  t_usage->max_tid = next_task_id - 1;
  while (!int32_t_buffer_is_empty(&ls_tids_buffer)) {
    t_usage->ms_run[int32_t_buffer_get(&ls_tids_buffer)] += int32_t_buffer_get(&ls_vals_buffer);
  }
}
