#include "test_router.h"

void test_router_same_sensor() {
  router_test_setup();

  location loc = { .sensor = sensor_offset('C', 8), .offset = 0 };
  reservation route[MAX_ROUTE_LENGTH];

  Assert(get_route(71, 10, &loc, &loc, route) == 0);

  Assert(route_length(route) == 1);
  Assert(route[0].train == 71);
  Assert(tstrcmp((char *)route[0].node->name, "C8"));
  Assert(route[0].ticks_end < 100);

  router_test_teardown();

  bwprintf("Success.\n\r");
}
