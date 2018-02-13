#include "k4.h"

void k4_first_user_task() {
  EnableCaches(true);

  int nameserver_tid = Create(MyPriority() + 3, &nameserver_main);
  Assert(nameserver_tid > 0);

  int clock_server_tid = Create(MyPriority() + 3, &clock_server);
  Assert(clock_server_tid > 0);

  int sender_tid = Create(MyPriority() + 5, &terminal_tx_server);
  Assert(sender_tid > 0);

  int idle_tid = Create(MyPriority() - 3, &idle_task_cursor);
  Assert(idle_tid > 0);

  Assert(Printf(sender_tid, "\033[2J") == 0);

  Assert(Create(MyPriority() + 1, &clock_view) > 0);
}
