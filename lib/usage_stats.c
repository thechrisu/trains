#include "usage_stats.h"

#ifndef VERSATILEPB
#define TIMER_DIVISOR 508.469
#else
#define TIMER_DIVISOR 1000
#endif /* VERSATILEPB */

void print_usage(void (*print)(char *,...), usage_stats* usage) {
  uint32_t total_runtime = 0;
  for (int i = 0; i < usage->max_tid; i++) {
    total_runtime += usage->ms_run[i] / TIMER_DIVISOR;
  }
  print("TOTAL: %d\n\r", total_runtime);
  for (int i = 0; i < usage->max_tid; i++) {
    int decimal = (unsigned int)(0.5 + (1000 * usage->ms_run[i] / TIMER_DIVISOR) / total_runtime) % 10;
    print("%d: \t%dms \t\t(%d.%d%%)\n\r", i, (unsigned int)(usage->ms_run[i] / TIMER_DIVISOR), (unsigned int)((100 * usage->ms_run[i] / TIMER_DIVISOR) / total_runtime), decimal);
  }
}
