#include "test_router.h"

void test_router_complete() {
  router_test_setup();

  location start = { .sensor = 0, .offset = 0 };
  location end = { .sensor = 0, .offset = 0 };
  track_node *route[MAX_ROUTE_LENGTH];

  for (int start_sensor = 0; start_sensor < 80; start_sensor += 1) {
    for (int end_sensor = 0; end_sensor < 80; end_sensor += 1) {
      location end_reverse;
      location_reverse(&track, &end_reverse, &end);

      start.sensor = start_sensor;
      end.sensor = end_sensor;

      if (sensor_reachable(&track, start_sensor, end_sensor) ||
          sensor_reachable(&track, start_sensor, end_reverse.sensor)) {
        Assert(get_route(&start, &end, route) == 0);
        Assert(route_length(route) > 0);
        Assert(route_length(route) <= 50);
      } else {
        Assert(get_route(&start, &end, route) == -1);
      }
    }
  }

  router_test_teardown();

  bwprintf("Success.\n\r");
}
