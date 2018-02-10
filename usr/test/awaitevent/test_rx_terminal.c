#include "test_rx_terminal.h"


void getter() {
  for (int i = 0; i < 4; i++) {
    bwgetc(TERMINAL);
  }
}

void test_rx_terminal() {
  ns_tid = Create(MyPriority() + 2, &nameserver_main);
  Assert(Create(MyPriority() - 3, &idle_task) > 0);
  Assert(Create(MyPriority() - 1, &getter) > 0);
  for (int i = 0; i < 5; i++) {
    int a = AwaitEvent(TERMINAL_RX_INTERRUPT);
    // __asm__("MOV pc, #0x20000000\n\t");
    bwprintf("TerminalRxInterrupt return code: %d\n\r", a);
  }
  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}
