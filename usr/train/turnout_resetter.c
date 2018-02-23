#include "turnout_resetter.h"

static int clock_server_tid;
static int tx_server_tid;
static int track_state_controller_tid;

static void switch_turnout_with_tids(int turnout_num, bool curved) {
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, turnout_num, curved);
}

void turnout_resetter() {
  clock_server_tid = WhoIs("ClockServer");
  tx_server_tid = WhoIs("TrainTxServer");
  track_state_controller_tid = WhoIs("TrackStateController");

  for (int i = 1; i <= 18; i += 1) {
    switch_turnout_with_tids(i, true);
  }
  switch_turnout_with_tids(153, true);
  switch_turnout_with_tids(154, false);
  switch_turnout_with_tids(155, true);
  switch_turnout_with_tids(156, false);
}
