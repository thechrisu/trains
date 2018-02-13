#include "idle.h"

void do_idle_loop(uint32_t *exp, const int c_server_tid) {
  int loops = 0;
  int32_t last_print = Time(c_server_tid);
  while (last_print + 25 > Time(c_server_tid)) {
    for (uint32_t i = 0; i < *exp; i++);
    loops++;
  }
  if (loops > 10)
    *exp *= loops / 5;
  if (loops > 5)
    *exp *= 1.0 + (loops - 5.0) / 100.0;
  if (loops <= 3)
    *exp /= 1.5;
}

void report_usage() {
  int32_t u = MyProcUsage();
  bwprintf("%d.%d%%\n\r", u / 10, u % 10);
}

void report_usage_cursor(int sender_tid) {
  int32_t u = MyProcUsage();
  Printf(sender_tid, "\033[H%d.%d%%\033[K", u / 10, u % 10);
}

void idle_task() {
  Assert(RegisterAs("Idle") == 0);
#if E2ETESTING
  while(1);
#else
  int c_server_tid = WhoIs("ClockServer");
  Assert(c_server_tid > 0);
  uint32_t exp = 1 << 20;
  while(1) {
    do_idle_loop(&exp, c_server_tid);
    report_usage();
  }
#endif /* E2ETESTING */
  Assert(0);
}

void idle_task_cursor() {
  Assert(RegisterAs("Idle") == 0);
#if E2ETESTING
  while(1);
#else
  int c_server_tid = WhoIs("ClockServer");
  Assert(c_server_tid > 0);
  int sender_tid = WhoIs("TerminalTxServer");
  Assert(sender_tid > 0);

  uint32_t exp = 1 << 20;
  while(1) {
    do_idle_loop(&exp, c_server_tid);
    report_usage_cursor(sender_tid);
  }
#endif /* E2ETESTING */
  Assert(0);
}
