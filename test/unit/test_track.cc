#include "test_track.h"
#include <gtest/gtest.h>

TEST(TestTrack, default_speeds_getset) {
  uint32_t m[81][15];

  default_speed real_defaults[2] = {
    {
      .train = 42,
      .speed = 7,
      .velocity = 50 * 10 * 100
    },
    {
      .train = 1337,
      .speed = 1337,
      .velocity = 1337
    }
  };

  default_speed super_defaults[15];
  super_defaults[14] = 1337;

  setup_speed_to_velocity_map(m, real_defaults, super_defaults);

  ASSERT_EQ(m[0][14], 1337);
  ASSERT_EQ(m[80][14], 1337);
  ASSERT_EQ(m[42][7], 50 * 10 * 100);
}
