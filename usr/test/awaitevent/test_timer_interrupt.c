#include "test_timer_interrupt.h"

void test_timer_interrupt() {
  ns_tid = Create(6, &nameserver_main);
  Create(2, &idle_task);
  Assert(AwaitEvent(-2) == -1);
  Assert(AwaitEvent(-1) == -1);
  //Assert(AwaitEvent(-2) == -1);
  for (int i = 0; i < 5; i++) {
    bwprintf("TEST\n\r");
    int r = AwaitEvent(TIMER_INTERRUPT);
    bwprintf("TEST\n\r");
    bwprintf("TimerInterrupt return code: %d\n\r", r); //AwaitEvent(TIMER_INTERRUPT));
  }
  int idle_tid = WhoIs("Idle");
  bwprintf("KILL: %d\n\r", idle_tid);
  Assert(Kill(idle_tid) == 0);
}
