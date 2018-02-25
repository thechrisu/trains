#include "velocity_calibrator.h"

void velocity_calibrator() {
  char sensor_letters[] = { 'D', 'E', 'D', 'B', 'C', 'A', 'B', 'C', 'B', 'C', 'E', 'E', 'C', 'A', 'B', 'C', 'B', 'D', 'E', 'E', 0 };
  int sensor_numbers[] =  {  5,   6,   4,   6,   12,  4,   16,  10,  3,   2,   2,   15,  12,  4,   16,  10,  1,   14,  14,  9,  0 };

  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));

  Assert(sender_tid == MyParentTid());
  Assert(received.type == MESSAGE_CALIB_V);

  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

  int train = received.msg.calib_v_params.train;
  int speed = 10;

  int clock_server_tid = WhoIs("ClockServer");
  int tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");
  int terminal_tx_server = WhoIs("TerminalTxServer");
  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s", ESC, CALIB_LINE, 1, "Calibrating velocity for train ", train, " and speed ", speed, HIDE_CURSOR_TO_EOL) == 0);

  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 6, true);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 10, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 15, true);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 16, true);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 17, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 155, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 156, true);

  stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, 13);
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('E', 10));
  stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, speed);

  int i = 0;
  while (sensor_letters[i]) {
    char letter = sensor_letters[i];
    char number = sensor_numbers[i];
    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset(letter, number));

    if (letter == 'B' && number == 3) {
      switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 16, false);
    }

    i += 1;
  }

  set_train_speed(tx_server_tid, track_state_controller_tid, train, 0);

  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%s", ESC, CALIB_LINE, 1, "Velocity calibration finished", HIDE_CURSOR_TO_EOL) == 0);
}

void straight_do_automated_velocity_calibration(int train, int speed, int track_state_controller_tid, int clock_server_tid, int tx_server_tid) {
  set_train_speed(tx_server_tid, track_state_controller_tid, train, speed);
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('D', 11));
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('C', 16));
  stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, 10);
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('D', 12));
  if (speed > 6 && speed != 14) {
    Delay(clock_server_tid, speed * 30);
  }
  stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, 0);
}

void automated_velocity_calibrator() {
  int sender_tid;
  message received;
  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));

  Assert(sender_tid == MyParentTid());
  Assert(received.type == MESSAGE_CALIB_V);

  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

  int train = received.msg.calib_v_params.train;

  int track_state_controller_tid = WhoIs("TrackStateController");
  int clock_server_tid = WhoIs("ClockServer");
  int tx_server_tid = WhoIs("TrainTxServer");

  int terminal_tx_server = WhoIs("TerminalTxServer");

  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 6, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 7, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 8, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 15, false);

  int num_repetitions = 1;
  for (int i = 0; i < num_repetitions; i++) {
    for (int speed = 1; speed < 15; speed++) {
      Assert(Printf(terminal_tx_server, "%s%d;%dH%s%d%s%d%s", ESC, CALIB_LINE, 1, "Calibrating velocity automatically for train ", train, " and speed ", speed, HIDE_CURSOR_TO_EOL) == 0);
      straight_do_automated_velocity_calibration(train, speed, track_state_controller_tid, clock_server_tid, tx_server_tid);
    }
  }

  Assert(Printf(terminal_tx_server, "%s%d;%dH%s%s", ESC, CALIB_LINE, 1, "Automatic velocity calibration finished", HIDE_CURSOR_TO_EOL) == 0);
  set_train_speed(tx_server_tid, track_state_controller_tid, train, 0);
}
