#include "clock_view.h"

void print_clock(int sender_tid, uint32_t deciseconds) {
  uint32_t minutes = deciseconds / 10 / 60;
  uint32_t seconds = (deciseconds / 10) % 60;
  deciseconds = deciseconds % 10;
  Printf(sender_tid, "\033[2;1H%d:%s%d.%d\033[K", minutes, seconds > 9 ? "" : "0", seconds, deciseconds);
}

void clock_view() {
  int clock_server_tid = WhoIs("ClockServer");
  int sender_tid = WhoIs("TerminalTxServer");
  uint32_t deciseconds = 0;

  while (true) {
    print_clock(sender_tid, deciseconds);
    deciseconds += 1;
    DelayUntil(clock_server_tid, 10 * deciseconds);
  }
}
