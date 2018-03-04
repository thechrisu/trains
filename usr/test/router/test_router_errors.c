#include "test_router.h"

void test_router_errors() {
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
  Assert(get_route(71, 10, &start, &end, route) == -1);
  Assert(get_route(71, 10, &start, &end, route) == -1);
  Assert(cancel_route(71) == 0);
  Assert(cancel_route(71) == -1);
  Assert(cancel_route(71) == -1);
  Assert(get_route(71, 10, &start, &end, route) == 0);
  Assert(get_route(71, 10, &start, &end, route) == -1);
  Assert(cancel_route(71) == 0);
  Assert(cancel_route(71) == -1);

  Assert(Kill(WhoIs("ClockNotifier")) == 0);

  bwprintf("Success.\n\r");
}
