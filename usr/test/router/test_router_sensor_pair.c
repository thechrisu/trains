#include "test_router.h"

void test_router_sensor_pair() {
  router_test_setup();

  location start = { .node = find_sensor(&track, sensor_offset('B', 16)), .offset = 0 };
  location end = { .node = find_sensor(&track, sensor_offset('E', 13)), .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  Assert(get_route(&start, &end, route) == 0);

  int length = route_node_count(route);
  Assert(length == 8);

  char *expected_sensors[] = { "B16", "BR15", "C10", "BR16", "B1", "D14", "MR17", "E14" };
  for (int i = 0; i < length; i += 1) {
    Assert(tstrcmp((char *)route[i]->name, expected_sensors[i]));
  }

  Assert(route_length(route) == 142100);

  router_test_teardown();

  bwprintf("Success.\n\r");
}
