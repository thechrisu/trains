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

  int clock_server_tid = WhoIs("ClockServer");
  int tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 6, true);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 10, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 15, true);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 16, true);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 155, false);
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 156, true);

  set_train_speed(tx_server_tid, track_state_controller_tid, train, 10);

  int i = 0;
  while (sensor_letters[i]) {
    char letter = sensor_letters[i];
    char number = sensor_numbers[i];
    poll_until_sensor_triggered(clock_server_tid, track_state_controller_tid, sensor_offset(letter, number));
    logprintf("Sensor %c%d triggered at %d ticks\n\r", letter, number, Time(clock_server_tid));

    if (letter == 'B' && number == 3) {
      switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, 16, false);
    }

    i += 1;
  }

  set_train_speed(tx_server_tid, track_state_controller_tid, train, 0);
}
