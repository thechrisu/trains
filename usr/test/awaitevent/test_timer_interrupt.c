#include "test_timer_interrupt.h"

void test_timer_interrupt() {
  ns_tid = Create(6, &nameserver_main);
  Create(2, &idle_task);
  Assert(AwaitEvent(-2) == -1);
  Assert(AwaitEvent(-1) == -1);
  for (int i = 0; i < 5; i++) {
    bwprintf("TimerInterrupt return code: %d\n\r", AwaitEvent(TIMER_INTERRUPT));
  }

  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
}
