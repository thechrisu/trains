#include "test_router.h"

void test_router_reversals() {
  router_test_setup();

  location start = { .sensor = sensor_offset('C', 9), .offset = 0 };
  location end = { .sensor = sensor_offset('A', 6), .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  Assert(get_route(&start, &end, route) == 0);

  int length = route_node_count(route);
  for (int i = 0; i < length; i++) {
    logprintf("%s\n\r", route[i]->name);
  }

  router_test_teardown();
  bwprintf("Success.\n\r");
}
