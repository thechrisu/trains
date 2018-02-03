#include "idle.h"

void idle_task() {
  Assert(RegisterAs("Idle") == 0);
  usage_stats last_seconds_stats;
  int idle_tid = MyTid();
  while(1) {
#ifndef E2ETESTING
    go_to_pos(1, 1);
#endif /* E2ETESTING */
    LastSecondsProcUsage(&last_seconds_stats);
    int32_t this_proc = last_seconds_stats.ms_run[idle_tid];
    int32_t total_time = 0;
    for (int i = 0; i < last_seconds_stats.max_tid; i++) {
      total_time += last_seconds_stats.ms_run[i];
    }
    int32_t fraction = (100 * this_proc) / total_time;
#ifndef E2ETESTING
    bwprintf("%d%%\n\r", fraction);
#endif /* E2ETESTING */
  }
  Assert(0);
}
