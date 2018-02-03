#include "test_clock.h"

void clock_syscall_errors() {
  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 1, &clock_server);

  bwprintf("Error code on negative delay: %d\n\r", Delay(clock_server_tid, -50));
  bwprintf("Error code on zero delay: %d\n\r", Delay(clock_server_tid, 0));
  bwprintf("Error code on delay to time in past/present: %d\n\r", DelayUntil(clock_server_tid, Time(clock_server_tid)));
  bwprintf("Error code on bad task ID: %d\n\r", Delay(-5, 125));

  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
}
