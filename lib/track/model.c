#include "model.h"

void setup_default_map(uint32_t map[81][15], default_value defaults[], uint32_t super_defaults[15]) {
  for (int i = 0; i < 81; i++) {
    for (int j = 0; j < 15; j++) {
      map[i][j] = super_defaults[j];
    }
  }

  default_value *current_value = (default_value *)defaults;
  while (!(current_value->train == 1337 && current_value->speed == 1337 && current_value->value == 1337)) {
    int t = current_value->train;
    int s = current_value->speed;
    uint32_t v = current_value->value;
    Assert(t >= 1 && t <= 81);
    Assert(s >= 0 && s <= 14);
    map[t][s] = v;
    current_value += 1;
  }
}
