#include "test_router.h"

void test_router_errors() {
  router_test_setup();

  location start = { .sensor = sensor_offset('C', 8), .offset = 0 };
  location end = { .sensor = sensor_offset('B', 12), .offset = 0 };
  reservation route[MAX_ROUTE_LENGTH];

  Assert(cancel_route(71) == -1);
  Assert(cancel_route(24) == -1);
  Assert(get_route(71, 10, &start, &end, route) == 0);
  Assert(get_route(71, 10, &start, &end, route) == -1);
  Assert(get_route(71, 10, &start, &end, route) == -1);
  Assert(get_route(24, 10, &start, &end, route) == 0);
  Assert(cancel_route(71) == 0);
  Assert(cancel_route(71) == -1);
  Assert(cancel_route(24) == 0);
  Assert(cancel_route(71) == -1);
  Assert(cancel_route(24) == -1);
  Assert(get_route(71, 10, &start, &end, route) == 0);
  Assert(get_route(71, 10, &start, &end, route) == -1);
  Assert(cancel_route(71) == 0);
  Assert(get_route(24, 10, &start, &end, route) == 0);
  Assert(cancel_route(71) == -1);

  router_test_teardown();

  bwprintf("Success.\n\r");
}
