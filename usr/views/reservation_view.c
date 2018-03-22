#include "reservation_view.h"

#define RES_VIEW_REFRESH_PERIOD 100

#define HEADER       "Train | Reservations"

#define LINE_CLEARED 1357
#define RES_COL      9

#define MAX_TRAINS   3

bool reservations_differ(message_reservation_get_all_response *left,
                         message_reservation_get_all_response *right) {
  if (left->count != right->count) {
    return true;
  }

  for (int i = 0; i < MAX_RESERVATIONS_RETURNED; i += 1) {
    if (left->reservations[i] != right->reservations[i]) {
      return true;
    }
  }

  return false;
}

static void clear_line(int terminal_tx_server_tid, int line) {
  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH      |%s",
                RESERVATION_LINE + 2 + line, 1, HIDE_CURSOR_TO_EOL) == 0);
};

void print_reservations(int terminal_tx_server_tid, int line,
                        message_reservation_get_all_response *res) {
  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH%s",
                RESERVATION_LINE + 2 + line, RES_COL,
                HIDE_CURSOR_TO_EOL) == 0);

  int col = RES_COL;

  for (int i = 0; i < res->count; i += 1) {
    int this_res = res->reservations[i];
    track_node *start = &track.track[RESERVATION_START(this_res)];
    track_node *end = &track.track[RESERVATION_END(this_res)];

    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH%s->%s%s",
                  RESERVATION_LINE + 2 + line, col,
                  start->name, end->name,
                  i < res->count - 1 ? ", " : "") == 0);
    col += tstrlen((char *)start->name) + 2 + tstrlen((char *)end->name) + 2;
  }
}

void reservation_view() {
  int clock_server_tid = WhoIs("ClockServer");
  Assert(clock_server_tid > 0);

  int terminal_tx_server_tid = WhoIs("TerminalTxServer");
  Assert(terminal_tx_server_tid > 0);

  int track_reservation_server_tid = WhoIs("TrackReservationServer");
  Assert(track_reservation_server_tid > 0);

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHReservation Revelation%s:%s",
                RESERVATION_LINE, 1, TRADEMARK, HIDE_CURSOR_TO_EOL) == 0);
  Assert(Printf(terminal_tx_server_tid,
                "\033[%d;%dH%s%s",
                RESERVATION_LINE + 1, 1,
                HEADER, HIDE_CURSOR_TO_EOL) == 0);

  int line_states[MAX_TRAINS];
  for (int i = 0; i < MAX_TRAINS; i += 1) {
    clear_line(terminal_tx_server_tid, i);
    line_states[i] = LINE_CLEARED;
  }

  message_reservation_get_all_response reservations[81];
  for (int i = 0; i < 81; i += 1) {
    reservations[i].count = 0;
  }

  int loops = 0;

  while (true) {
    for (int i = 0; i < num_active_trains; i += 1) {
      int train = active_trains[i];

      message_reservation_get_all_response res;
      reservation_get_all(track_reservation_server_tid, train, &res);

      if (line_states[i] != train) {
        clear_line(terminal_tx_server_tid, i);
        Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH   %s%d",
                      RESERVATION_LINE + 2 + i, 1,
                      train < 10 ? " " : "", train) == 0);
      }

      if (reservations_differ(&res, &reservations[train]) ||
          line_states[i] != train) {
        tmemcpy(&reservations[train], &res, sizeof(reservations[train]));
        print_reservations(terminal_tx_server_tid, i, &reservations[train]);
      }

      line_states[i] = train;
    }

    for (int i = num_active_trains; i < MAX_TRAINS; i += 1) {
      if (line_states[i] != LINE_CLEARED) {
        clear_line(terminal_tx_server_tid, i);
        line_states[i] = LINE_CLEARED;
      }
    }

    loops += 1;
    DelayUntil(clock_server_tid, RES_VIEW_REFRESH_PERIOD * loops);
  }
}
