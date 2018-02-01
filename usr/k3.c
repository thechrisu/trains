#include "k3.h"

void k3_first_user_task() {
  Create(7, &nameserver_main);
  Create(2, &idle_task);

  message send;
  send.type = MESSAGE_DELAY_UNTIL;

  int clock_server_tid = Create(5, &clock_server);

  for (int i = 0; i < 10; i += 1) {
    send.msg.message_delay_until_ticks = 100 * (i + 1);
    Send(clock_server_tid, &send, sizeof(send), EMPTY_MESSAGE, 0);
    bwprintf("Seconds: %d\n\r", i + 1);
  }

  Kill(WhoIs("ClockNotifier"));
  Kill(WhoIs("Idle"));
}
