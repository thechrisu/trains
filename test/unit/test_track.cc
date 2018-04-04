#include "test_track.h"
#include <gtest/gtest.h>

TEST(TestTrack, default_speeds_getset) {
  uint32_t m[81][15];

  default_value real_defaults[2] = {
    {
      .train = 42,
      .speed = 7,
      .value = 50 * 10 * 100
    },
    {
      .train = 1337,
      .speed = 1337,
      .value = 1337
    }
  };

  uint32_t super_defaults[15];
  super_defaults[14] = 1337;

  setup_default_map(m, real_defaults, super_defaults);

  ASSERT_EQ(m[0][14], 1337);
  ASSERT_EQ(m[80][14], 1337);
  ASSERT_EQ(m[42][7], 50 * 10 * 100);
}

#define SWITCH_DIST 25000

TEST(TestTrack, rebase) {
  int turnouts[] = { 6, 7, 8, 9, 11, 14 };
  int num_turnouts = 6;

  turnout_state turnout_states[NUM_TURNOUTS];

  track_state track;
  init_track(&track);

  for (int i = 0; i < num_turnouts; i++) {
    location l = {
      .node = turnout_num_to_node(&track, turnouts[i]),
      .offset = -SWITCH_DIST
    };
    location r;
    location_reverse(&r, &l);
    track_node *n = track_node_next(r.node, turnout_states);
    location_rebase(turnout_states, &l, &l);
    ASSERT_GE(l.offset, 0);
    ASSERT_NE(l.node, nullptr);
  }
}
