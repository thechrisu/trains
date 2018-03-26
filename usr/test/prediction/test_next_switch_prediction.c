#include "test_next_switch_prediction.h"

void test_next_switch_prediction() {
  router_test_setup();
  coordinates c, r;
  coordinates next;
  int next_num;
  bool is_curved;
  bool found;

  track_node *route[MAX_ROUTE_LENGTH];
  c.loc.node = find_sensor(&track, sensor_offset('B', 15));
  c.loc.offset = 0;
  r.loc.node = find_sensor(&track, sensor_offset('A', 3));
  r.loc.offset = 0;
  Assert(sensor_reachable(&track, c.loc.node->num, r.loc.node->num));
  Assert(get_route(&c.loc, &r.loc, route) == 0);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 0, 10);
  Assert(!found);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1000000, 10);
  Assert(!found);

  r.loc.node = find_node_by_name(&track, "C13");
  Assert(get_route(&c.loc, &r.loc, route) == 0);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1000000, 10);
  Assert(found);
  Assert(next.loc.offset == 0);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1000000, 0);
  Assert(!found);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 0, 10);
  Assert(found);
  Assert(next_num == 14);
  Assert(!is_curved);
  Assert(next.loc.offset == 0);
  Assert(next.loc.node == find_node_by_name(&track, "BR14"));
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1, 10);
  Assert(found);
  Assert(next_num == 14);
  Assert(!is_curved);
  Assert(next.loc.offset > 0);
  Assert(next.loc.node == find_node_by_name(&track, "A3"));

  r.loc.node = find_node_by_name(&track, "C5");
  Assert(get_route(&c.loc, &r.loc, route) == 0);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1000000, 10);
  Assert(found);
  Assert(next.loc.offset == 0);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1000000, 0);
  Assert(!found);
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 0, 10);
  Assert(found);
  Assert(next_num == 14);
  Assert(is_curved);
  Assert(next.loc.offset == 0);
  Assert(next.loc.node == find_node_by_name(&track, "BR14"));
  predict_next_switch(&c, route, &next, &next_num, &is_curved, &found, 1, 10);
  Assert(found);
  router_test_teardown();
  bwprintf("Success.\n\r");
}
