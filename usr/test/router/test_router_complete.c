#include "test_router.h"

void test_router_complete() {
  router_test_setup();

  location start = { .sensor = 0, .offset = 0 };
  location end = { .sensor = 0, .offset = 0 };
  reservation route[MAX_ROUTE_LENGTH];

  for (int start_sensor = 0; start_sensor < 80; start_sensor += 1) {
    for (int end_sensor = 0; end_sensor < 80; end_sensor += 1) {
      location end_reverse;
      location_reverse(&track, &end_reverse, &end);

      if (sensor_reachable(&track, start_sensor, end_sensor) ||
          sensor_reachable(&track, start_sensor, end_reverse.sensor)) {
        start.sensor = start_sensor;
        end.sensor = end_sensor;
        Assert(get_route(58, 12, &start, &end, route) == 0);
        Assert(route_length(route) > 0);
        Assert(route_length(route) <= 50);
        Assert(cancel_route(58) == 0);
      }
    }
  }

  router_test_teardown();

  bwprintf("Success.\n\r");
}
