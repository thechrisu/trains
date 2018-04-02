#include "test_collision_prediction.h"

void test_collision_prediction() {
  router_test_setup();

  int whatever14 = (int)track.speed_to_velocity[78][14];
  coordinates c = { .loc = { .node = find_node_by_name(&track, "C6"), .offset = 0 }, .ticks = 0,
                    .current_speed = 14, .velocity = whatever14,
                    .target_velocity = whatever14, .acceleration = 0 };
  int o_vol = (int)track.speed_to_velocity[24][14];
  coordinates o = { .loc = { .node = find_node_by_name(&track, "C9"), .offset = 0 }, .ticks = 0,
                    .current_speed = 14, .velocity = o_vol,
                    .target_velocity = o_vol, .acceleration = 0 };
  turnout_state turnout_states[NUM_TURNOUTS];
  get_turnouts(WhoIs("TrackStateController"), turnout_states);
  bool r;
  coordinates temp, me_temp;
  tmemcpy(&temp, &o, sizeof(temp));
  tmemcpy(&me_temp, &c, sizeof(me_temp));
  r = will_collide_with_other_train(0, &me_temp, &temp, 1, turnout_states);
  Assert(!r);
  tmemcpy(&temp, &o, sizeof(temp));
  tmemcpy(&me_temp, &c, sizeof(me_temp));
  r = will_collide_with_other_train(100000, &me_temp, &temp, 1, turnout_states);
  Assert(r);
  tmemcpy(&temp, &o, sizeof(temp));
  tmemcpy(&me_temp, &c, sizeof(me_temp));
  r = will_collide_with_other_train(100000, &me_temp, &temp, 0, turnout_states);
  Assert(!r);

  c.loc.node = find_node_by_name(&track, "C5");
  r = will_collide_with_other_train(100000, &c, &o, 1, turnout_states);
  Assert(!r);

  bwprintf("Success.\n\r");
  router_test_teardown();
}
