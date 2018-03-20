#include "test_train_coordinates_server.h"

void train_coordinates_server_test_setup() {
  int my_priority = MyPriority();

  int idle_tid = Create(MyPriority() - 3, &idle_task);
  Assert(idle_tid > 0);

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = Create(my_priority + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);

  int train_tx_server_tid = Create(my_priority + 2, &train_tx_server);
  Assert(train_tx_server_tid > 0);
}

void train_coordinates_server_test_teardown() {
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(WhoIs("TrainTxNotifier")) == 0);
  Assert(Kill(WhoIs("Idle")) == 0);
}

void test_train_coordinates_server() {
  int train = 24;
  coordinates c;

  train_coordinates_server_test_setup();

  int clock_server_tid = WhoIs("ClockServer");
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = WhoIs("TrackStateController");
  Assert(track_state_controller_tid > 0);

  int train_coords_server_tid = WhoIs("TrainCoordinatesServer");
  Assert(train_coords_server_tid > 0);

  int train_tx_server_tid = WhoIs("TrainTxServer");
  Assert(train_tx_server_tid > 0);

  // Location is uninitialized at beginning
  get_coordinates(train_coords_server_tid, 1, &c);
  Assert(c.loc.sensor == NO_NEXT_SENSOR);
  get_coordinates(train_coords_server_tid, 80, &c);
  Assert(c.loc.sensor == NO_NEXT_SENSOR);

  // Setting the train's speed initializes speed, velocity, and acceleration, but not location
  set_train_speed(train_tx_server_tid, track_state_controller_tid, train, 6);

  Delay(clock_server_tid, 25);
  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.sensor == NO_NEXT_SENSOR);
  Assert(c.ticks >= 25 && c.ticks <= 28);
  Assert(c.current_speed == 6);
  Assert(c.last_speed == 0);
  Assert(c.velocity > 0);
  Assert(c.target_velocity > 0);
  Assert(c.velocity < c.target_velocity);
  Assert(c.acceleration > 0);

  // Train is at full speed
  Delay(clock_server_tid, 125);
  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.sensor == NO_NEXT_SENSOR);
  Assert(c.ticks >= 100 && c.ticks <= 400);
  Assert(c.current_speed == 6);
  Assert(c.last_speed == 0);
  Assert(c.target_velocity > 0);
  Assert(c.velocity == c.target_velocity);
  Assert(c.acceleration == 0);

  // Hitting a sensor sets the train's location
  message send;
  send.type = MESSAGE_UPDATE_COORDS_SENSOR;
  send.msg.update_coords.tr_data.train = train;
  send.msg.update_coords.last_sensor.sensor = sensor_offset('B', 15);
  send.msg.update_coords.last_sensor.ticks = Time(clock_server_tid);
  Assert(Send(train_coords_server_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);

  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.sensor == sensor_offset('B', 15));

  int last_offset = c.loc.offset;
  Assert(c.loc.offset >= 0);

  Assert(c.current_speed == 6);
  Assert(c.last_speed == 0);
  Assert(c.target_velocity > 0);
  Assert(c.velocity == c.target_velocity);
  Assert(c.acceleration == 0);

  // Slowing down and stopping
  set_train_speed(train_tx_server_tid, track_state_controller_tid, train, 0);

  Delay(clock_server_tid, 25);
  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.sensor == sensor_offset('B', 15));

  Assert(c.loc.offset >= 0);
  Assert(c.loc.offset > last_offset);
  last_offset = c.loc.offset;

  Assert(c.current_speed == 0);
  Assert(c.last_speed == 6);
  Assert(c.velocity > 0);
  Assert(c.target_velocity == 0);
  Assert(c.velocity > c.target_velocity);
  Assert(c.acceleration < 0);

  // Train has come to a stop
  Delay(clock_server_tid, 150);
  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.sensor == sensor_offset('B', 15));

  Assert(c.loc.offset >= 0);
  Assert(c.loc.offset > last_offset);
  last_offset = c.loc.offset;

  Assert(c.ticks >= 200 && c.ticks <= 500);
  Assert(c.current_speed == 0);
  Assert(c.last_speed == 6);
  Assert(c.velocity == 0);
  Assert(c.target_velocity == 0);
  Assert(c.acceleration == 0);

  // Train is in same location after another 0.5 seconds
  Delay(clock_server_tid, 50);
  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.offset == last_offset);

  // Reversing
  reverse_train(train_tx_server_tid, track_state_controller_tid, train);

  get_coordinates(train_coords_server_tid, train, &c);
  Assert(c.loc.sensor == sensor_offset('B', 16));
  Assert(c.loc.offset == -last_offset + PICKUP_LENGTH);
  last_offset = c.loc.offset;

  train_coordinates_server_test_teardown();

  bwprintf("Success.\n\r");
}
