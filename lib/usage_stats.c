#include "usage_stats.h"

void print_usage(void (*print)(char *,...), usage_stats* usage) {
  int32_t total_runtime = 0;
  for (int i = 0; i < usage->max_tid; i++) {
    total_runtime += usage->ms_run[i];
  }
  for (int i = 0; i < usage->max_tid; i++) {
    print("%d: \t%d ticks \t\t(%d%%)\n\r", i, usage->ms_run[i], (100 * usage->ms_run[i]) / total_runtime);
  }
}
