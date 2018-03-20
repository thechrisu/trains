#include "test_route_execution.h"

void route_execution_test_setup() {
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = Create(my_priority + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);

  int router_tid = Create(my_priority + 1, &router);
  Assert(router_tid > 0);
}

void route_execution_test_teardown() {
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
}

void test_route_execution_trivial() {
  track_node *route[MAX_ROUTE_LENGTH];
  location end = { .sensor = sensor_offset('C', 3), .offset = 0 };

  location start1 = { .sensor = sensor_offset('B', 10), .offset = 0 };
  Assert(get_route(&start1, &end, route) == 0);
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, INFINITE_DISTANCE));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, INFINITE_DISTANCE));

  location start2 = { .sensor = sensor_offset('B', 9), .offset = 0 };
  Assert(get_route(&start2, &end, route) == 0);
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, INFINITE_DISTANCE));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, INFINITE_DISTANCE));
}

void test_route_execution_no_reversal_because_none_in_route() {
  track_node *route[MAX_ROUTE_LENGTH];
  location end = { .sensor = sensor_offset('C', 5), .offset = 0 };

  location start1 = { .sensor = sensor_offset('D', 12), .offset = 0 };
  Assert(get_route(&start1, &end, route) == 0);
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, INFINITE_DISTANCE));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, INFINITE_DISTANCE));

  start1.offset = 100;
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &start1, INFINITE_DISTANCE));

  location start2 = { .sensor = sensor_offset('E', 11), .offset = 0 };
  Assert(get_route(&start1, &end, route) == 0);
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, INFINITE_DISTANCE));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &end, INFINITE_DISTANCE));

  start2.offset = 100;
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, 0));
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, 100000));
  Assert(!is_reverse_in_distance((track_node**)&route, &start2, INFINITE_DISTANCE));
}

void test_route_execution_tricky_cases() {

}

void test_route_execution_helpers() {
  route_execution_test_setup();
  test_route_execution_trivial();
  test_route_execution_no_reversal_because_none_in_route();
  route_execution_test_teardown();
  bwprintf("Success.\n\r");
}
