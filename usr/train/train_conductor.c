#include "train_conductor.h"

void conductor_setspeed(int train_tx_server, int track_state_controller, int clock_server,
                        train_data *d, int speed) {
  d->last_speed = d->should_speed;
  d->time_speed_last_changed = Time(clock_server);
  d->should_speed = speed;
  set_train_speed(train_tx_server, track_state_controller,
                  d->train, d->should_speed);
}

void conductor_reverse(int train_tx_server, int track_state_controller, int clock_server,
                       train_data *d) {
  d->last_speed = d->should_speed;
  d->direction = !d->direction;
  stop_and_reverse_train(clock_server, train_tx_server,
                         track_state_controller, d->train);
  d->time_speed_last_changed = Time(clock_server);
}

// Only performs one iteration of calibration
void conductor_calib_sd_one_iter(int train_tx_server, int track_state_controller,
                                 int clock_server, train_data *d, int speed) {
  // TODO precompute distance from D9 to B10
  set_train_speed_and_headlights(train_tx_server, track_state_controller,
                                 d->train, 0, true);
  unsigned int start_sensor, goal_sensor;
  if (speed == 14) {
    start_sensor = sensor_offset('D', 9);
    goal_sensor = sensor_offset('B', 10);
  } else if (speed <= 9) {
    start_sensor = sensor_offset('D', 11);
    goal_sensor = sensor_offset('C', 16);
  } else if (speed >= 12) {
    start_sensor = sensor_offset('E', 12);
    goal_sensor = sensor_offset('B', 10);
  } else {
    start_sensor = sensor_offset('C', 8);
    goal_sensor = sensor_offset('B', 10);
  }
  int d = 100 * distance_between_sensors_helper(find_sensor(start_sensor),
                                                find_sensor(goal_sensor),
                                                0, 15);
  // sensor_offset('D' 9), sensor_offset('B', 10)
  // go back twice stopping distance (once: until start sensor)
  // then decelarate
  // then reverse + accelerate to speed
  // poll until we hit the start sensor D9
  // compute (distance_to_b10 - estimated_stopping_distance)
  // compute stopping time, as well
  // then wait a while
  // poll B10?
  // if B10 hit, compute diff (time), new stopping distance = old * 0.9 + diff * c
  // if no B10 hit, crawl forward at speed 1 until we hit it, record time.
  // update stopping distance/time estimate
}

void conductor_calib_sd(int train_tx_server, int track_state_controller,
                        int clock_server, train_data *d, int speed) {
  switch_turnout(clock_server, train_tx_server, track_state_controller, 3, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 5, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 7, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 8, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 9, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 11, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 18, false);

  for (int i = 0; i < 5; i++) {
    conductor_calib_sd_one_iter(train_tx_server, track_state_controller,
                                clock_server, d, speed);
  }
}

void train_conductor() {
  int sender_tid;
  message received, ready;
  int train_tx_server = WhoIs("TrainTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int cmd_dispatcher = WhoIs("CommandDispatcher");
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);
  Assert(cmd_dispatcher > 0);

  train_data d;
  d.last_speed = 0;
  d.time_speed_last_changed = Time(clock_server);

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
  Assert(received.msg.train > 0 && received.msg.train <= 80);
  Assert(received.type == MESSAGE_CONDUCTOR_SETTRAIN);
  d.train = received.msg.train;

  ready.msg.train = d.train;
  ready.type = MESSAGE_READY;
  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
    switch (received.type) {
      case MESSAGE_USER:
        switch (received.msg.cmd.type) {
          case USER_CMD_SD:
            logprintf("NotYetImplemented(SD for train %d)\n\r", d.train);
            Delay(clock_server, 10000); // Just to test automode/manual mode, you can remove this later.
            break;
          case USER_CMD_TR:
            Assert(received.msg.cmd.data[0] == d.train);
            conductor_setspeed(train_tx_server, track_state_controller, clock_server,
                               &d, received.msg.cmd.data[1]);
            break;
          case USER_CMD_RV:
            Assert(received.msg.cmd.data[0] == d.train);
            conductor_reverse(train_tx_server, track_state_controller, clock_server, &d);
            break;
          default:
            logprintf("Got user cmd message of type %d\n\r", received.msg.cmd.type);
            Assert(0);
            break;
        }
        Assert(Send(cmd_dispatcher, &ready, sizeof(ready), EMPTY_MESSAGE, 0) == 0);
        break;
      default:
        logprintf("Got user cmd message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
