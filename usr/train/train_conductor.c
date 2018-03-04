#include "train_conductor.h"

void conductor_setspeed(int train_tx_server, int track_state_controller, int clock_server,
                        train_data *d, int speed) {
  d->last_speed = d->should_speed;
  d->time_speed_last_changed = Time(clock_server);
  d->should_speed = speed;
  set_train_speed(train_tx_server, track_state_controller,
                  d->train, d->should_speed);
}

void conductor_reverse_to_speed(int train_tx_server,
                                int track_state_controller, int clock_server,
                                train_data *d, int speed) {
  d->last_speed = d->should_speed;
  d->direction = !d->direction;
  stop_and_reverse_train_to_speed(clock_server, train_tx_server,
                                  track_state_controller, d->train, speed);
  d->should_speed = speed;
  d->time_speed_last_changed = Time(clock_server);
}

// Only performs one iteration of calibration
void conductor_calib_sd_one_iter(int train_tx_server, int track_state_controller,
                                 int clock_server, train_data *d, int speed) {
  message velocity_model, stopping_distance_model, stopping_time_model;
  get_constant_velocity_model(track_state_controller, d->train,
                              &velocity_model);
  get_stopping_distance_model(track_state_controller, d->train,
                              &stopping_distance_model);
  get_stopping_time_model(track_state_controller, d->train,
                          &stopping_time_model);

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
  int dist = 99 * distance_between_sensors_helper(find_sensor(&track, start_sensor),
                                                  find_sensor(&track, goal_sensor),
                                                  0, 15);
  // First, go back until we hit the start sensor
  conductor_reverse_to_speed(train_tx_server, track_state_controller,
                             clock_server, d, 10);
  // TODO(culshoefer) pick reverse sensor
  poll_until_sensor_triggered(clock_server, track_state_controller,
                              start_sensor);
  conductor_reverse_to_speed(train_tx_server, track_state_controller,
                             clock_server, d, speed);
  poll_until_sensor_triggered(clock_server, track_state_controller,
                              start_sensor);
  int before_stopping = Time(clock_server);
  while (dist - stopping_distance_model.msg.train_distances[speed] > 0) {
    Delay(clock_server, 1);
    dist -= velocity_model.msg.train_speeds[speed] *
            (Time(clock_server) - before_stopping) / 100; // divide by 100 to get seconds
  }
  int stop_start = Time(clock_server);
  conductor_setspeed(train_tx_server, track_state_controller, clock_server,
                     d, 0);
  int did_hit_sensor = !poll_until_sensor_triggered_with_timeout(
                                              clock_server,
                                              track_state_controller,
                                              goal_sensor,
                                              speed * 50);
  int ticks_after_poll = Time(clock_server);
  if (did_hit_sensor) {
    int ticks_to_stop = ticks_after_poll - stop_start;
    stopping_distance_model.msg.train_distances[speed]
      -= stopping_time_model.msg.train_times[speed] - ticks_to_stop;
    update_stopping_distance_model(track_state_controller, d->train, speed,
                                   stopping_distance_model.msg.train_distances[speed]);
    update_stopping_time_model(track_state_controller, d->train, speed,
                               ticks_to_stop * 10000);
  } else {
    conductor_setspeed(train_tx_server, track_state_controller, clock_server,
                       d, 1);
    poll_until_sensor_triggered(clock_server, track_state_controller, goal_sensor);
    conductor_setspeed(train_tx_server, track_state_controller, clock_server,
                       d, 0);
    int distance_off = velocity_model.msg.train_speeds[speed]
                         * (Time(clock_server) - ticks_after_poll) / 100;
    update_stopping_distance_model(track_state_controller, d->train, speed,
                                   stopping_distance_model.msg.train_distances[speed] + distance_off);
  }
}

void conductor_calib_sd(int train_tx_server, int track_state_controller,
                        int clock_server, train_data *d, int speed) {
  switch_turnout(clock_server, train_tx_server, track_state_controller, 3, false);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 5, true);
  switch_turnout(clock_server, train_tx_server, track_state_controller, 7, speed >= 10);
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
            conductor_reverse_to_speed(train_tx_server, track_state_controller,
                                       clock_server, &d, d.should_speed);
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
