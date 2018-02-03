#include "test_clock.h"

void assert_within_n_ticks(uint32_t milliseconds, uint32_t ticks, int n) {
  Assert(milliseconds >= 10 * (ticks - n) && milliseconds <= 10 * (ticks + n));
}
