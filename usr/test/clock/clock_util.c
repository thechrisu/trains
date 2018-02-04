#include "test_clock.h"

void assert_within_tick_range(uint32_t milliseconds, uint32_t ticks, int below, int above) {
  Assert(milliseconds >= 10 * (ticks - below) && milliseconds <= 10 * (ticks + above));
}
