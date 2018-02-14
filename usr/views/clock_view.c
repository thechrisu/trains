#include "clock_view.h"

void print_clock(int sender_tid, uint32_t deciseconds) {
  uint32_t minutes = deciseconds / 10 / 60;
  uint32_t seconds = (deciseconds / 10) % 60;
  deciseconds = deciseconds % 10;
  Printf(sender_tid, "%s%d;%dH%d:%s%d.%d%s", ESC, CLOCK_LINE, 1, minutes, seconds > 9 ? "" : "0", seconds, deciseconds, HIDE_CURSOR_TO_EOL);
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
