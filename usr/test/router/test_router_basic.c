#include "test_router.h"

void test_router_basic() {
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = Create(my_priority + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);

  int router_tid = Create(my_priority + 1, &router);
  Assert(router_tid > 0);

  location start = { .sensor = sensor_offset('C', 8), .offset = 0 };
  location end = { .sensor = sensor_offset('B', 12), .offset = 0 };
  reservation route[MAX_ROUTE_LENGTH];

  Assert(get_route(71, 10, &start, &end, route) == 0);

  reservation *current = route;
  int length = 0;
  while (current->train != 0) {
    Assert(current->train == 71);
    current += 1;
    length += 1;
  }

  Assert(length == 5);

  Assert(tstrcmp(route[0].node->name, "C8"));
  Assert(tstrcmp(route[1].node->name, "BR3"));
  Assert(tstrcmp(route[2].node->name, "BR2"));
  Assert(tstrcmp(route[3].node->name, "A7"));
  Assert(tstrcmp(route[4].node->name, "B12"));

  int reservation_length = route[0].ticks_end - route[0].ticks_start;
  Assert(route[1].ticks_end - route[1].ticks_start == reservation_length);
  Assert(route[2].ticks_end - route[2].ticks_start == reservation_length);
  Assert(route[3].ticks_end - route[3].ticks_start == reservation_length);
  Assert(route[4].ticks_end - route[4].ticks_start == reservation_length);

  Assert(route[4].ticks_end < 500);

  Assert(Kill(WhoIs("ClockNotifier")) == 0);

  bwprintf("Success.\n\r");
}
