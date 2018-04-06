#include "anti_freezer.h"

void anti_freezer() {
  int clock_server = WhoIs("ClockServer");
  Assert(clock_server > 0);

  Delay(clock_server, 200); // To allow the other tasks to run

  int idle = WhoIs("Idle");
  Assert(idle > 0);

  usage_stats last_second;
  while (true) {
    Delay(clock_server, 200);

    LastSecondsProcUsage(&last_second);
    if (last_second.ms_run[idle % MAX_TASKS] < 1) {
      Assert(0 && "Froze!");
    }
  }
}
