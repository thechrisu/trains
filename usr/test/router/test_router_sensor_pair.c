#include "test_router.h"

void test_router_sensor_pair() {
  router_test_setup();

  location start = { .sensor = sensor_offset('B', 16), .offset = 0 };
  location end = { .sensor = sensor_offset('E', 13), .offset = 0 };
  reservation route[MAX_ROUTE_LENGTH];

  Assert(get_route(24, 5, &start, &end, route) == 0);

  int length = route_length(route);
  Assert(length == 8);

  char *expected_sensors[] = { "B16", "BR15", "C10", "BR16", "B1", "D14", "MR17", "E14" };
  for (int i = 0; i < length; i += 1) {
    Assert(tstrcmp((char *)route[i].node->name, expected_sensors[i]));
    Assert(route[i].train == 24);
  }

  int reservation_length = route[0].ticks_end - route[0].ticks_start;
  for (int i = 1; i < length; i += 1) {
    Assert(route[i].ticks_end - route[i].ticks_start == reservation_length);
  }

  Assert(route[length - 1].ticks_end < 1400);

  router_test_teardown();

  bwprintf("Success.\n\r");
}
