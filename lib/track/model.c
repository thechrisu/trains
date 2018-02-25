#include "model.h"

void setup_speed_to_velocity_map(uint32_t map[81][15], default_speed defaults[], uint32_t super_defaults[15]) {
  for (int i = 0; i < 81; i++) {
    for (int j = 0; j < 15; j++) {
      map[i][j] = super_defaults[j];
    }
  }

  default_speed *current_speed = (default_speed *)defaults;
  while (!(current_speed->train == 1337 && current_speed->speed == 1337 && current_speed->velocity == 1337)) {
    int t = current_speed->train;
    int s = current_speed->speed;
    uint32_t v = current_speed->velocity;
    Assert(t >= 1 && t <= 81);
    Assert(s >= 0 && s <= 14);
    Assert(v < DEFINITE_MAX_CM_PER_SEC * 10 * 100);
    map[t][s] = v;
    current_speed += 1;
  }
}
