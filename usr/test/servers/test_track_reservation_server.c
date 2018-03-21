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

void assert_get_all_index(message_reservation_get_all_response res,
                          int index, char *start_name, char *end_name) {
  track_node *start = find_node_by_name(&track, start_name);
  track_node *end = find_node_by_name(&track, end_name);
  int start_index = node_index_in_track_state(&track, start);
  int end_index = node_index_in_track_state(&track, end);
  Assert(res.reservations[index] == RESERVATION_ENCODE(start_index, end_index));
}

void test_track_reservation_server() {
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  Assert(Create(my_priority + 1, &track_state_controller) > 0);

  int trs = Create(my_priority + 1, &track_reservation_server);
  Assert(trs > 0);

  message_reservation_get_all_response res;

  // Reservations are exclusive
  assert_reservation_make(trs, 1, "A5", "MR3", 0);
  assert_reservation_make(trs, 1, "A5", "MR3", -2);
  assert_reservation_make(trs, 2, "A5", "MR3", -2);
  assert_reservation_make(trs, 2, "MR3", "C7", 0);
  assert_reservation_make(trs, 1, "MR3", "C7", -2);
  assert_reservation_make(trs, 2, "MR3", "C7", -2);

  reservation_get_all(trs, 1, &res);
  Assert(res.count == 2);
  assert_get_all_index(res, 0, "A5", "MR3");
  assert_get_all_index(res, 1, "BR3", "A6");

  reservation_get_all(trs, 2, &res);
  Assert(res.count == 2);
  assert_get_all_index(res, 0, "C8", "BR3");
  assert_get_all_index(res, 1, "MR3", "C7");

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

  reservation_get_all(trs, 1, &res);
  Assert(res.count == 10);

  reservation_drop_all(trs, 1);
  reservation_drop_all(trs, 2);

  reservation_get_all(trs, 1, &res);
  Assert(res.count == 0);
  reservation_get_all(trs, 2, &res);
  Assert(res.count == 0);

  assert_reservation_make(trs, 1, "C7", "MR18", 0);
  assert_reservation_make(trs, 2, "MR18", "BR5", 0);
  assert_reservation_make(trs, 1, "BR5", "C3", 0);
  assert_reservation_make(trs, 2, "BR5", "MR7", 0);

  reservation_get_all(trs, 1, &res);
  Assert(res.count == 4);
  reservation_get_all(trs, 2, &res);
  Assert(res.count == 4);

  reservation_drop_all(trs, 1);
  reservation_drop_all(trs, 2);

  reservation_get_all(trs, 1, &res);
  Assert(res.count == 0);
  reservation_get_all(trs, 2, &res);
  Assert(res.count == 0);

  // Reserving edge prevents reservations of reverse
  assert_reservation_make(trs, 1, "BR3", "A6", 0);
  assert_reservation_make(trs, 1, "A5", "MR3", -2);
  assert_reservation_make(trs, 2, "A5", "MR3", -2);

  // Dropping edge allows reservations of reverse
  assert_reservation_drop(trs, 1, "BR3", "A6", 0);
  assert_reservation_make(trs, 1, "A5", "MR3", 0);

  bwprintf("Success.\n\r");
}
