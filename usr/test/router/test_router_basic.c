#include "test_router.h"

void test_router_basic() {
  router_test_setup();

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

  char *expected_sensors[] = { "C8", "BR3", "BR2", "A7", "B12" };
  for (int i = 0; i < length; i += 1) {
    Assert(tstrcmp((char *)route[i].node->name, expected_sensors[i]));
  }

  int reservation_length = route[0].ticks_end - route[0].ticks_start;
  for (int i = 1; i < length; i += 1) {
    Assert(route[i].ticks_end - route[i].ticks_start == reservation_length);
  }

  Assert(route[length - 1].ticks_end < 500);

  router_test_teardown();

  bwprintf("Success.\n\r");
}
