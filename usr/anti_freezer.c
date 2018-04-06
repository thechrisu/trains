#include "anti_freezer.h"

void anti_freezer() {
  int clock_server = WhoIs("ClockServer");
  Delay(clock_server, 200); // To allow the other tasks to run
  int idle = WhoIs("Idle");


  usage_stats last_second;
  while (true) {
    Delay(clock_server, 2000);

    LastSecondsProcUsage(&last_second);

    if (last_second.ms_run[idle % MAX_TASKS] < 5) {
      Assert(0 && "Froze!");
    }
  }
}
