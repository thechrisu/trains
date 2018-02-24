#include "stopping_distance_calibrator.h"

void stopping_distance_calibrator() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));

  Assert(sender_tid == MyParentTid());
  Assert(received.type == MESSAGE_CALIB_SD);

  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

  int train = received.msg.calib_sd_params.train;
  int speed = received.msg.calib_sd_params.speed;

  int clock_server_tid = WhoIs("ClockServer");
  int tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 3, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 18, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 5, false);

  // HACK: unless the last run was (likely) at speed 12, 13, or 14, move train forward to make sure it's over C7 sensor
  if (speed != 13 && speed != 14 && speed != 1) {
    set_train_speed(tx_server_tid, track_state_controller_tid, train, 10);
    Delay(clock_server_tid, 200);
  }

  set_train_speed(tx_server_tid, track_state_controller_tid, train, 0);
  Delay(clock_server_tid, 200);
  reverse_train(tx_server_tid, track_state_controller_tid, train);
  set_train_speed(tx_server_tid, track_state_controller_tid, train, 8);

  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, 'C', speed < 6 ? 7 : 3);

  set_train_speed(tx_server_tid, track_state_controller_tid, train, 0);
  Delay(clock_server_tid, 330);
  reverse_train(tx_server_tid, track_state_controller_tid, train);
  set_train_speed(tx_server_tid, track_state_controller_tid, train, speed);

  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, 'C', 8);

  set_train_speed_and_headlights(tx_server_tid, track_state_controller_tid, train, 0, true);
}
