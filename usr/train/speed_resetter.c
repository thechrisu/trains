#include "speed_resetter.h"

static int tx_server_tid;
static int track_state_controller_tid;

static void set_speed_with_tids(int train, int speed) {
  set_train_speed(tx_server_tid, track_state_controller_tid, train, speed);
}

void speed_resetter() {
  tx_server_tid = WhoIs("TrainTxServer");
  track_state_controller_tid = WhoIs("TrackStateController");

  for (int i = 1; i <= 80; i += 1) {
    set_speed_with_tids(i, 0);
  }
}
