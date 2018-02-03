#include "test_clock.h"

int clock_server_tid;

void time_syscall_delay(uint32_t ticks) {
  uint32_t start_time, elapsed_time;
  char *s = ticks == 1 ? "" : "s";

  bwprintf("Delaying %d tick%s\n\r", ticks, s);

  start_time = get_time();
  Assert(Delay(clock_server_tid, ticks) == 0);
  elapsed_time = get_time() - start_time;

  bwprintf("Actual time for delaying %d tick%s: %d ms\n\r", ticks, s, elapsed_time);
  assert_within_n_ticks(elapsed_time, ticks, 1);
}

void time_syscall_delay_until(uint32_t ticks) {
  uint32_t start_time, elapsed_time;
  uint32_t ticks_at_start;
  char *s = ticks == 1 ? "" : "s";

  bwprintf("Delaying until %d tick%s from now\n\r", ticks, s);

  start_time = get_time();

  ticks_at_start = Time(clock_server_tid);
  Assert(ticks_at_start > 0);
  Assert(DelayUntil(clock_server_tid, ticks_at_start + ticks) == 0);

  elapsed_time = get_time() - start_time;

  bwprintf("Actual time for delaying until %d tick%s from then: %d ms\n\r", ticks, s, elapsed_time);
  assert_within_n_ticks(elapsed_time, ticks, 1);
}

void clock_syscall_accuracy() {
  uint32_t start_time, start_milliseconds, elapsed_time, elapsed_milliseconds;

  setup_timer();

  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  clock_server_tid = Create(MyPriority() + 1, &clock_server);
  Create(2, &idle_task);

  start_milliseconds = get_time();
  start_time = Time(clock_server_tid);

  time_syscall_delay(1);
  time_syscall_delay(10);
  time_syscall_delay(50);
  time_syscall_delay_until(5);
  time_syscall_delay_until(10);
  time_syscall_delay_until(50);

  elapsed_time = Time(clock_server_tid) - start_time;
  elapsed_milliseconds = get_time() - start_milliseconds;

  bwprintf("Entire task took %d ticks (actual time %d milliseconds)\n\r", elapsed_time, elapsed_milliseconds);
  assert_within_n_ticks(elapsed_milliseconds, elapsed_time, 2);

  Assert(Kill(WhoIs("Idle")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
}
