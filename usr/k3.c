#include "k3.h"

void k3_first_user_task() {
  EnableCaches(true);
  Create(7, &nameserver_main);
  Create(2, &idle_task);
  int clock_server_tid = Create(5, &clock_server);

  for (int i = 0; i < 10; i += 1) {
    DelayUntil(clock_server_tid, 100 * (i + 1));
    bwprintf("Seconds: %d\n\r", i + 1);
  }

  Kill(WhoIs("ClockNotifier"));
  Kill(WhoIs("Idle"));
}
