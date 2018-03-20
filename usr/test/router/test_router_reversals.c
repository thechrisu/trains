#include "test_router.h"

void test_single_reversal() {
  location start = { .sensor = sensor_offset('C', 9), .offset = 0 };
  location end = { .sensor = sensor_offset('C', 5), .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  Assert(get_route(&start, &end, route) == 0);

  int length = route_node_count(route);
  Assert(length == 4);
  char *expected_segments[] = { "C9", "MR15", "BR15", "C5"};
  for (int i = 0; i < length; i++) {
    Assert(tstrcmp((char *)route[i]->name, expected_segments[i]));
  }
}

void test_double_reversal() {
  location start = { .sensor = sensor_offset('C', 9), .offset = 0 };
  location end = { .sensor = sensor_offset('A', 6), .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  Assert(get_route(&start, &end, route) == 0);

  int length = route_node_count(route);
  char *expected_segments[] = { "C9", "MR15", "BR15", "C5", "BR6", "MR18",
                                "BR18", "C8", "BR3", "A6" };
  Assert(length == 10);
  for (int i = 0; i < length; i++) {
    Assert(tstrcmp((char *)route[i]->name, expected_segments[i]));
  }
}


void test_router_reversals() {
  router_test_setup();
  test_single_reversal();
  test_double_reversal();
  bwprintf("Success.\n\r");
  router_test_teardown();
}
