#include "turnout_view.h"

static int terminal_tx_server_tid;

void update_turnout_view(int index, turnout_state state) {
  int line = TURNOUTS_HEADING_LINE + 1 + index % 11;
  bool second_column = index > 10;
  int column = TURNOUTS_COLUMN + (second_column ? 9 : 0);
  unsigned int turnout_num = map_offset_to_turnout((unsigned int)index);
  char state_char = state == TURNOUT_UNKNOWN ? ' ' : state == TURNOUT_STRAIGHT ? 'S' : 'C';

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH%s%d | %c",
                line, column, turnout_num >= 100 ? "" : turnout_num >= 10 ? " " : "  ", turnout_num, state_char) == 0);
}

void init_turnout_view() {
  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHTurnouts:%s",
                TURNOUTS_HEADING_LINE, TURNOUTS_COLUMN + 1, HIDE_CURSOR_TO_EOL) == 0);

  for (int i = 1; i < NUM_TURNOUTS / 2; i += 1) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH    |       |%s",
                  TURNOUTS_HEADING_LINE + i, TURNOUTS_COLUMN, HIDE_CURSOR_TO_EOL) == 0);
  }

  for (int i = 0; i < NUM_TURNOUTS; i += 1) {
    update_turnout_view(i, TURNOUT_UNKNOWN);
  }
}

void turnout_view() {
  terminal_tx_server_tid = WhoIs("TerminalTxServer");
  Assert(terminal_tx_server_tid > 0);

  int event_server = WhoIs("EventServer");
  Assert(event_server > 0);

  Subscribe(event_server, EVENT_TURNOUT_SWITCHED);

  init_turnout_view();

  event e;
  while (true) {
    ReceiveEvent(&e);
    Assert(e.type == EVENT_TURNOUT_SWITCHED);

    int turnout_index = turnout_num_to_map_offset(e.body.turnout.number);
    update_turnout_view(turnout_index, e.body.turnout.state);
  }
}
