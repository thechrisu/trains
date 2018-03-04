#include "test_router.h"

void test_router_sensor_pair() {
  int my_priority = MyPriority();

  ns_tid = Create(my_priority + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int clock_server_tid = Create(my_priority + 2, &clock_server);
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = Create(my_priority + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);

  int router_tid = Create(my_priority + 1, &router);
  Assert(router_tid > 0);

  location start = { .sensor = sensor_offset('B', 16), .offset = 0 };
  location end = { .sensor = sensor_offset('E', 13), .offset = 0 };
  reservation route[MAX_ROUTE_LENGTH];

  Assert(get_route(24, 5, &start, &end, route) == 0);

  reservation *current = route;
  int length = 0;
  while (current->train != 0) {
    Assert(current->train == 24);
    current += 1;
    length += 1;
  }

  Assert(length == 8);

  char *expected_sensors[] = { "B16", "BR15", "C10", "BR16", "B1", "D14", "MR17", "E14" };
  for (int i = 0; i < length; i += 1) {
    Assert(tstrcmp((char *)route[i].node->name, expected_sensors[i]));
  }

  int reservation_length = route[0].ticks_end - route[0].ticks_start;
  for (int i = 1; i < length; i += 1) {
    Assert(route[i].ticks_end - route[i].ticks_start == reservation_length);
  }

  Assert(route[length - 1].ticks_end < 1000);

  Assert(Kill(WhoIs("ClockNotifier")) == 0);

  bwprintf("Success.\n\r");
}
