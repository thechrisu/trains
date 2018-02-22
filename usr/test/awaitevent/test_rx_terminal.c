#include "test_rx_terminal.h"


void getter() {
  for (int i = 0; i < 5; i++) {
    Assert(bwgetc(TERMINAL) >= 0);
  }
}

void test_rx_terminal() {
  Assert(Create(MyPriority() - 1, &getter) > 0);
  for (int i = 0; i < 5; i++) {
    int a = AwaitEvent(TERMINAL_RX_INTERRUPT);
    // __asm__("MOV pc, #0x20000000\n\t");
    bwprintf("TerminalRxInterrupt return code: %d\n\r", a);
  }
}
