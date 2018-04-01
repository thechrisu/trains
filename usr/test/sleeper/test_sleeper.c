#include "test_sleeper.h"

#define ALARM_TICKS 5

void test_sleeper() {
  int my_priority = MyPriority();
  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);
  Create(my_priority - 1, &idle_task);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);
  int before_delay = Time(clock_server_tid);
  set_alarm(ALARM_TICKS);
  int sleeper_tid = -1;
  message empty;
  Assert(Receive(&sleeper_tid, &empty, sizeof(empty)) == sizeof(empty));
  Assert(empty.type == MESSAGE_WAKEUP);
  int after_delay = Time(clock_server_tid);
  Assert(after_delay - before_delay >= ALARM_TICKS);
  Assert(Reply(sleeper_tid, EMPTY_MESSAGE, 0) == 0);
  Assert(Kill(WhoIs("Idle")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
  bwprintf("Success.\n\r");
}
