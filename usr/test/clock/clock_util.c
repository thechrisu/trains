#include "test_clock.h"

void assert_at_most_n_ticks_greater(uint32_t milliseconds, uint32_t ticks, int n) {
  Assert(milliseconds >= 10 * ticks && milliseconds <= 10 * (ticks + n));
}
