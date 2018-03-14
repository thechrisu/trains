#include "test_train_coordinates_server.h"

void train_coordinates_server_test_setup() {
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = Create(my_priority + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);
}

void train_coordinates_server_test_teardown() {
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
}

void test_train_coordinates_server() {
  train_coordinates_server_test_setup();

  int train_coords_server_tid = WhoIs("TrainCoordinatesServer");
  Assert(train_coords_server_tid > 0);

  coordinates c;

  // Location is uninitialized at beginning
  get_coordinates(train_coords_server_tid, 1, &c);
  Assert(c.loc.sensor == NO_NEXT_SENSOR);
  get_coordinates(train_coords_server_tid, 80, &c);
  Assert(c.loc.sensor == NO_NEXT_SENSOR);

  train_coordinates_server_test_teardown();

  bwprintf("Success.");
}
