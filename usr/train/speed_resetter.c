#include "speed_resetter.h"

void speed_resetter() {
  int tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  for (int i = 0; i < num_active_trains; i += 1) {
    set_train_speed(tx_server_tid, track_state_controller_tid,
                    active_trains[i], 0);
  }
}
