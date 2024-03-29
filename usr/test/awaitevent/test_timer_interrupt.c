#include "test_timer_interrupt.h"

void test_timer_interrupt() {
  ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(Create(MyPriority() - 3, &idle_task) > 0);
  Assert(AwaitEvent(-2) == -1);
  Assert(AwaitEvent(-1) == -1);
  for (int i = 0; i < 5; i++) {
    bwprintf("TimerInterrupt return code: %d\n\r", AwaitEvent(TIMER_INTERRUPT));
  }

  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}
