#include "test_clock.h"

void clock_syscall_errors() {
  message send, reply;

  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 1, &clock_server);

  bwprintf("Response type on negative delay: %d\n\r", Delay(-50));
  bwprintf("Response type on zero delay: %d\n\r", Delay(0));
  bwprintf("Response type on delay to time in past/present: %d\n\r", DelayUntil(Time()));

  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
}
