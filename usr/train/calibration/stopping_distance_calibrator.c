#include "stopping_distance_calibrator.h"

/**
  TODO Autocalibrate
  - Go from calibration in the back to the front and vice versa
  - Repeat this multiple times
*/

void stopping_distance_calibrator() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));

  Assert(sender_tid == MyParentTid());
  Assert(received.type == MESSAGE_CALIB_SD);

  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

  int train = received.msg.calib_sd_params.train;
  int speed = received.msg.calib_sd_params.speed;

  int terminal_tx_server = WhoIs("TerminalTxServer");
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s", ESC, CALIB_LINE, 1, "Calibrating stopping distance for train ", train, " and speed ", speed, HIDE_CURSOR_TO_EOL) == 0);

  int clock_server_tid = WhoIs("ClockServer");
  int tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  if (speed == 14) {
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 9, false);
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 11, false);
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 12, false);
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 16, false);

    stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, 8);

    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('C', 13));

    stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, speed);
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 11, true);

    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('E', 8));
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 11, false);

    set_train_speed_and_headlights(tx_server_tid, track_state_controller_tid, train, 0, true);
  } else {
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 3, false);
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 18, false);
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 5, false);

    // HACK: unless the last run was (likely) at 8-14, move train forward to make sure it's over C7 sensor
    if (speed != 1 && speed < 9) {
      set_train_speed(tx_server_tid, track_state_controller_tid, train, 10);
      Delay(clock_server_tid, 200);
    }

    stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, 8);

    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('C', speed < 6 ? 7 : 3));

    stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, speed);

    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('C', 8));

    set_train_speed_and_headlights(tx_server_tid, track_state_controller_tid, train, 0, true);
  }

  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%s", ESC, CALIB_LINE, 1, "Stopping distance calibration finished", HIDE_CURSOR_TO_EOL) == 0);
}
