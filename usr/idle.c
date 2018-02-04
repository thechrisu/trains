#include "idle.h"

void idle_task() {
  RegisterAs("Idle");
  int c_server_tid = WhoIs("ClockServer");
  Assert(c_server_tid > 0);
  int32_t exp = 1 << 18;
  while(1) {
    int loops = 0;
    int32_t last_print = Time(c_server_tid);
    while (last_print + 100 > Time(c_server_tid)) {
      for (uint32_t i = 0; i < exp; i++);
      loops++;
    }
    if (loops > 5)
      exp *= 1.05;
    if (loops <= 1)
      exp /= 1.05;
#ifndef E2ETESTING
    go_to_pos(1, 1);
    bwprintf("%d%%\n\r", MyProcUsage());
#endif /* E2ETESTING */
  }
  Assert(0);
}
