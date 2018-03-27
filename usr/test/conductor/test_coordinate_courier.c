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

void test_coordinates_to_notification_basic() {
  track_state t;
  coordinates current, last;
  location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE];
  bool is_location_set[MAX_LOCATIONS_TO_OBSERVE];
  location_notification new;

  current.loc.node = NULL_TRACK_NODE;
  last.loc.node = NULL_TRACK_NODE;
  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == GOT_LOST);

  last.loc.node = &t.tracka[0];
  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == GOT_LOST);
}

void test_coordinates_to_notification_complex() {
  track_state t;
  init_track(&t);
  coordinates current, last;
  location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE];
  bool is_location_set[MAX_LOCATIONS_TO_OBSERVE];
  location_notification new;

  locations_to_observe[0].loc.node = &t.tracka[0];
  current.loc.node = &t.tracka[0];
  last.loc.node = NULL_TRACK_NODE;

  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == LOCATION_CHANGED);
  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == LOCATION_CHANGED);
  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == LOCATION_CHANGED);
  Assert(new.loc.node == &t.tracka[0]);

  is_location_set[0] = true;
  locations_to_observe[0].reason = LOCATION_TO_STOP;
  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == LOCATION_TO_STOP);

  last.loc.node = current.loc.node;
  Assert(!coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(!coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.loc.node == &t.tracka[0]);

  current.loc.node = find_node_by_name(&t, "BR15");
  locations_to_observe[0].loc.node = current.loc.node;
  locations_to_observe[0].loc.offset = current.loc.offset;
  last.loc.node = NULL_TRACK_NODE;
  locations_to_observe[0].reason = LOCATION_TO_SWITCH;
  locations_to_observe[0].switch_to_switch[0] = 187; // Should not matter
  locations_to_observe[0].switch_to_switch[1] = 186; // Should not matter
  is_location_set[0] = true;
  Assert(coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
  Assert(new.reason == LOCATION_TO_SWITCH);
  Assert(new.switch_to_switch[0] == 187);
  Assert(new.switch_to_switch[1] == 186);
  Assert(new.loc.node == find_node_by_name(&t, "BR15"));
  last.loc.node = current.loc.node;
  Assert(!coordinates_to_notification(&current, &last, locations_to_observe, is_location_set, &new));
}

void test_coord_courier() {
  test_coord_courier_not_located_yet();
  test_coordinates_to_notification_basic();
  test_coordinates_to_notification_complex();
  bwprintf("Success.\n\r");
}
