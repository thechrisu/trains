#include "test_coordinate_courier.h"

int coord_courier_test_setup() {
  int demo_train = 42;
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = Create(my_priority + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);

  // spawn clock server, train coordinate server
  return create_courier(demo_train);
}

void coord_courier_test_teardown(int courier_tid) {
  Assert(Kill(courier_tid) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
}

void test_coord_courier_not_located_yet() {
  int courier_tid = coord_courier_test_setup();
  int receiver_tid;
  message received;
  Assert(Receive(&receiver_tid, &received, sizeof(received)) == sizeof(received));
  Assert(receiver_tid == courier_tid);

  Assert(received.msg.notification_response.reason == GOT_LOST);
  Assert(received.msg.notification_response.loc.node == NULL_TRACK_NODE);
  coord_courier_test_teardown(courier_tid);
}

void test_coord_courier() {
  test_coord_courier_not_located_yet();
  bwprintf("Success.\n\r");
}
