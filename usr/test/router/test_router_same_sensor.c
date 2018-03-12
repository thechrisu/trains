#include "test_router.h"

void test_router_same_sensor() {
  router_test_setup();

  location loc = { .sensor = sensor_offset('C', 8), .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  Assert(get_route(&loc, &loc, route) == 0);

  Assert(route_length(route) == 1);
  Assert(tstrcmp((char *)route[0]->name, "C8"));

  router_test_teardown();

  bwprintf("Success.\n\r");
}
