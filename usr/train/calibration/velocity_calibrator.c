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
  int speed = 14;

  int clock_server_tid = WhoIs("ClockServer");
  int tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

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

  int last_time, this_time;
  int i = 0;
  while (sensor_letters[i]) {
    char letter = sensor_letters[i];
    char number = sensor_numbers[i];
    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset(letter, number));

    this_time = Time(clock_server_tid);

    if (i > 0) {
      unsigned int start = sensor_offset(sensor_letters[i - 1], sensor_numbers[i - 1]);
      unsigned int end = sensor_offset(letter, number);
      update_constant_velocity_model(track_state_controller_tid, train, speed, start, end, this_time - last_time);
    }

    last_time = this_time;

    if (letter == 'B' && number == 3) {
      switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 16, false);
    }

    i += 1;
  }

  set_train_speed(tx_server_tid, track_state_controller_tid, train, 0);
}

void straight_do_automated_velocity_calibration(int train, int speed, int track_state_controller_tid, int clock_server_tid, int tx_server_tid) {
  int first_time, second_time;
  set_train_speed(tx_server_tid, track_state_controller_tid, train, speed);
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('D', 11));
  first_time = Time(clock_server_tid);
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('C', 16));
  second_time = Time(clock_server_tid);

  unsigned int start = sensor_offset('D', 11);
  unsigned int end = sensor_offset('C', 16);
  update_constant_velocity_model(track_state_controller_tid, train, speed, start, end, second_time - first_time);

  stop_and_reverse_train_to_speed(clock_server_tid, tx_server_tid, track_state_controller_tid, train, 10);
  poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset('D', 12));
  if (speed > 6) {
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

  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 6, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 7, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 8, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 15, false);

  straight_do_automated_velocity_calibration(train, 13, track_state_controller_tid, clock_server_tid, tx_server_tid);
  straight_do_automated_velocity_calibration(train, 14, track_state_controller_tid, clock_server_tid, tx_server_tid);
  straight_do_automated_velocity_calibration(train, 1, track_state_controller_tid, clock_server_tid, tx_server_tid);
  int num_repetitions = 1;
  for (int i = 0; i < num_repetitions; i++) {
    for (int speed = 1; speed < 15; speed++) {
    }
  }
}
