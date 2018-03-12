#include "test_router.h"

void test_router_basic() {
  router_test_setup();

  location start = { .sensor = sensor_offset('C', 8), .offset = 0 };
  location end = { .sensor = sensor_offset('B', 12), .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  Assert(get_route(&start, &end, route) == 0);

  int length = route_length(route);
  Assert(length == 5);

  char *expected_sensors[] = { "C8", "BR3", "BR2", "A7", "B12" };
  for (int i = 0; i < length; i += 1) {
    Assert(tstrcmp((char *)route[i]->name, expected_sensors[i]));
  }

  router_test_teardown();

  bwprintf("Success.\n\r");
}
