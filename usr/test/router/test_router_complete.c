#include "test_router.h"

void test_router_complete() {
  router_test_setup();

  location start = { .sensor = 0, .offset = 0 };
  location end = { .sensor = 0, .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  for (int start_sensor = 0; start_sensor < 80; start_sensor += 1) {
    for (int end_sensor = 0; end_sensor < 80; end_sensor += 1) {
      start.sensor = start_sensor;
      end.sensor = end_sensor;

      location end_reverse;
      location_reverse(&track, &end_reverse, &end);

      if (sensor_reachable(&track, start_sensor, end_sensor) ||
          sensor_reachable(&track, start_sensor, end_reverse.sensor)) {
        Assert(get_route(&start, &end, route) == 0);
        Assert(route_node_count(route) > 0);
        Assert(route_node_count(route) <= 50);
      } else {
        Assert(get_route(&start, &end, route) == -1);
      }
    }
  }

  router_test_teardown();

  bwprintf("Success.\n\r");
}
