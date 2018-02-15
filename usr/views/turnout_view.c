#include "turnout_view.h"

static int terminal_tx_server_tid, track_state_controller_tid;
static turnout_state turnout_states[NUM_TURNOUTS];

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
  message send, reply;

  send.type = MESSAGE_GETTURNOUTS;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
  Assert(reply.type == REPLY_GETTURNOUTS);

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHTurnouts%s",
                TURNOUTS_HEADING_LINE, TURNOUTS_COLUMN, HIDE_CURSOR_TO_EOL) == 0);

  for (int i = 1; i < NUM_TURNOUTS / 2; i += 1) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH    |       |%s",
                  TURNOUTS_HEADING_LINE + i, TURNOUTS_COLUMN, HIDE_CURSOR_TO_EOL) == 0);
  }

  for (int i = 0; i < NUM_TURNOUTS; i += 1) {
    turnout_state initial_state = reply.msg.turnout_states[i];
    turnout_states[i] = initial_state;
    update_turnout_view(i, initial_state);
  }
}

void turnout_view() {
  message send, reply;

  send.type = MESSAGE_GETTURNOUTS;

  int clock_server_tid = WhoIs("ClockServer");
  terminal_tx_server_tid = WhoIs("TerminalTxServer");
  track_state_controller_tid = WhoIs("TrackStateController");

  int initial_ticks = Time(clock_server_tid);
  int loops = 0;

  init_turnout_view();

  while (true) {
    Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
    Assert(reply.type == REPLY_GETTURNOUTS);

    for (int i = 0; i < NUM_TURNOUTS; i += 1) {
      turnout_state old = turnout_states[i];
      turnout_state new = reply.msg.turnout_states[i];
      if (old != new) {
        update_turnout_view(i, new);
      }
      turnout_states[i] = new;
    }

    loops += 1;
    DelayUntil(clock_server_tid, initial_ticks + REFRESH_PERIOD * loops);
  }
}
