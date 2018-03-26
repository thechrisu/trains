#include "test_conductor.h"

void conductor_test_setup() {
  router_test_setup();
}

void conductor_test_teardown() {
  router_test_teardown();
}

void test_craft_triggers() {
  conductor_test_setup();
  int train = 42;
  int track_state_controller = WhoIs("TrackStateController");
  Assert(track_state_controller > 0);

  message velocity_model;
  get_constant_velocity_model(track_state_controller, train,
                              &velocity_model);

  message stopping_distance_model;
  get_stopping_distance_model(track_state_controller, train,
                              &stopping_distance_model);

  coordinates c;
  c.loc.node = find_node_by_name(&track, "B15");
  c.loc.offset = 2; // Should not matter, really
  c.target_velocity = 140000;
  c.velocity = 140000;
  c.acceleration = 0;
  c.current_speed = 14;
  Assert(c.loc.node != NULL_TRACK_NODE);

  track_node *route[MAX_ROUTE_LENGTH];
  location start = { .node = find_sensor(&track, sensor_offset('C', 9)), .offset = 0 };
  location end = { .node = find_sensor(&track, sensor_offset('C', 6)), .offset = 0 };
  Assert(get_route(&start, &end, route) == 0);
  Assert(route_node_count(route) > 0);

  location_notification locs[MAX_LOCATIONS_TO_OBSERVE];
  int n_reqs;

  craft_new_triggers(&c, velocity_model.msg.train_speeds,
      stopping_distance_model.msg.train_distances, route, locs, &n_reqs);
  Assert(n_reqs > 0);
  Assert(locs[n_reqs - 1].reason == LOCATION_TO_STOP);
  location_notification exp[3] = {
    { .reason = 2, .loc = { .node = find_node_by_name(&track, "B15"), .offset = find_node_by_name(&track, "B15")->edge[DIR_AHEAD].dist * 100}},
    { .reason = 2, .loc = { .node = find_node_by_name(&track, "BR14"), .offset = find_node_by_name(&track, "BR14")->edge[DIR_CURVED].dist  * 100}},
    { .reason = 3, .loc = { .node = find_node_by_name(&track, "BR8"), .offset = find_node_by_name(&track, "BR8")->edge[DIR_AHEAD].dist * 100}},
  };
  for (int i = 0; i < n_reqs; i++) {
    Assert(locs[i].reason == exp[i].reason);
    Assert(locs[i].loc.node == exp[i].loc.node);
    Assert(locs[i].loc.offset < exp[i].loc.offset);
  }
  conductor_test_teardown();
}

void test_conductor() {
  test_craft_triggers();
  bwprintf("Success.\n\r");
}
