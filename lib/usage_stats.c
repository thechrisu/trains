#include "usage_stats.h"

#ifndef VERSATILEPB
#define TIMER_DIVISOR 508.469
#else
#define TIMER_DIVISOR 1000
#endif /* VERSATILEPB */

void print_usage(void (*print)(char *,...), usage_stats* usage) {
  uint32_t total_runtime = 0;
  for (int i = 0; i <= usage->max_tid; i++) {
    total_runtime += usage->ms_run[i] / TIMER_DIVISOR;
  }
  print("TOTAL: %d\n\r", total_runtime);
  for (int i = 0; i <= usage->max_tid; i++) {
    unsigned int scaled = (unsigned int)(0.5 + (1000.0 * usage->ms_run[i] / TIMER_DIVISOR) / total_runtime);
    unsigned int frac = scaled / 10;
    unsigned int decimal = scaled % 10;
    print("%d: \t%dms \t\t(%d.%d%%)\n\r", i, (unsigned int)(0.5 + ((usage->ms_run[i]) / TIMER_DIVISOR)), frac, decimal);
  }
}
