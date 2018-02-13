#include "test_printf.h"

void printf_errors() {
  bwprintf("Terminal transmit server not yet up: %d\n\r", Printf(-1, "Test"));

  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 3, &clock_server);
  int idle_tid = Create(MyPriority() - 3, &idle_task);
  Assert(idle_tid > 0);
  int sender_tid = Create(MyPriority() + 1, &terminal_tx_server);
  Assert(sender_tid >= 0);

  char long_string[] = "This is a very long string. "
                       "This is a very long string. "
                       "This is a very long string. "
                       "This is a very long string. "
                       "This is a very long string. "
                       "This is a very long string. "
                       "This is a very long string. "
                       "This is a very long string. ";
  bwprintf("Printing too long of a string: %d\n\r", Printf(sender_tid, "%s%s", long_string, long_string));

  Assert(Kill(sender_tid) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(WhoIs("TerminalTxNotifier")) == 0);
  Assert(Kill(clock_server_tid) == 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}
