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

void printf_happy_path() {
  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 3, &clock_server);
  int idle_tid = Create(MyPriority() - 3, &idle_task);
  Assert(idle_tid > 0);
  int sender_tid = Create(MyPriority() + 1, &terminal_tx_server);
  Assert(sender_tid >= 0);

  Printf(sender_tid, "");
  Printf(sender_tid, "%s", "");
  Printf(sender_tid, "Char: %c\n\r", 'c');
  Printf(sender_tid, "String: %s\n\r", "Hello world");
  Printf(sender_tid, "Unsigned integer: %u\n\r", -1);
  Printf(sender_tid, "Signed integers: %d %d %d\n\r", 0, 5, -5);
  Printf(sender_tid, "Hex numbers: %x %x\n\r", 0x1234, 0xDEADBEEF);
  Printf(sender_tid, "%%%% %s\n\r", "<- should be two percent signs");
  Printf(sender_tid, "All at once: %c %s %u %d %x %%\n\r", 'a', "b", -1, -1, 0x5678ABCD);

  Assert(Kill(sender_tid) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(WhoIs("TerminalTxNotifier")) == 0);
  Assert(Kill(clock_server_tid) == 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}
