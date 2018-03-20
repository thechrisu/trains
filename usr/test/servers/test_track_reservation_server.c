#include "test_track_reservation_server.h"

void assert_reservation_make(int trs, int train, char *start, char *end, int result) {
  track_node *start_node = find_node_by_name(&track, start);
  track_node *end_node = find_node_by_name(&track, end);
  Assert(start_node != NULL_TRACK_NODE);
  Assert(end_node != NULL_TRACK_NODE);
  Assert(reservation_make(trs, train, start_node, end_node) == result);
}

void assert_reservation_drop(int trs, int train, char *start, char *end, int result) {
  track_node *start_node = find_node_by_name(&track, start);
  track_node *end_node = find_node_by_name(&track, end);
  Assert(start_node != NULL_TRACK_NODE);
  Assert(end_node != NULL_TRACK_NODE);
  Assert(reservation_drop(trs, train, start_node, end_node) == result);
}

void test_track_reservation_server() {
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  Assert(Create(my_priority + 1, &track_state_controller) > 0);

  int trs = Create(my_priority + 1, &track_reservation_server);
  Assert(trs > 0);

  // Reservations are exclusive
  assert_reservation_make(trs, 1, "A5", "MR3", 0);
  assert_reservation_make(trs, 1, "A5", "MR3", -2);
  assert_reservation_make(trs, 2, "A5", "MR3", -2);
  assert_reservation_make(trs, 2, "MR3", "C7", 0);
  assert_reservation_make(trs, 1, "MR3", "C7", -2);
  assert_reservation_make(trs, 2, "MR3", "C7", -2);

  // Can't reserve nonexistent edge
  assert_reservation_make(trs, 2, "MR3", "MR18", -1);

  // Can't drop nonexistent edge
  assert_reservation_drop(trs, 1, "MR3", "MR18", -1);

  // Can't drop unowned edge
  assert_reservation_drop(trs, 1, "MR3", "C7", -2);
  assert_reservation_drop(trs, 2, "A5", "MR3", -2);
  assert_reservation_drop(trs, 1, "C7", "MR18", -2);

  // Can drop owned edge
  assert_reservation_drop(trs, 1, "A5", "MR3", 0);
  assert_reservation_drop(trs, 2, "MR3", "C7", 0);

  // Can re-reserve dropped edge
  assert_reservation_make(trs, 1, "MR3", "C7", 0);
  assert_reservation_make(trs, 2, "A5", "MR3", 0);

  // Can drop all edges
  assert_reservation_make(trs, 1, "C7", "MR18", 0);
  assert_reservation_make(trs, 1, "MR18", "BR5", 0);
  assert_reservation_make(trs, 1, "BR5", "C3", 0);
  assert_reservation_make(trs, 1, "BR5", "MR7", 0);
  reservation_drop_all(trs, 1);
  assert_reservation_make(trs, 1, "C7", "MR18", 0);
  assert_reservation_make(trs, 2, "MR18", "BR5", 0);
  assert_reservation_make(trs, 1, "BR5", "C3", 0);
  assert_reservation_make(trs, 2, "BR5", "MR7", 0);
  reservation_drop_all(trs, 1);
  reservation_drop_all(trs, 2);

  // Reserving edge prevents reservations of reverse
  assert_reservation_make(trs, 1, "BR3", "A6", 0);
  assert_reservation_make(trs, 1, "A5", "MR3", -2);
  assert_reservation_make(trs, 2, "A5", "MR3", -2);

  // Dropping edge allows reservations of reverse
  assert_reservation_drop(trs, 1, "BR3", "A6", 0);
  assert_reservation_make(trs, 1, "A5", "MR3", 0);

  bwprintf("Success.\n\r");
}
