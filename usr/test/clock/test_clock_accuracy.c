#include "test_clock.h"

int clock_server_tid;

void time_delay(uint32_t ticks) {
  message send;
  uint32_t start_time, elapsed_time;
  char *s = ticks == 1 ? "" : "s";

  send.type = MESSAGE_DELAY;
  send.msg.message_delay_ticks = ticks;

  bwprintf("Delaying %d tick%s\n\r", ticks, s);

  start_time = get_time();
  Assert(Send(clock_server_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
  elapsed_time = get_time() - start_time;

  bwprintf("Actual time for delaying %d tick%s: %d ms\n\r", ticks, s, elapsed_time);
  Assert(elapsed_time >= 10 * (ticks - 1) && elapsed_time <= 10 * (ticks + 1));
}

void clock_accuracy() {
  setup_timer();

  ns_tid = Create(7, &nameserver_main);
  clock_server_tid = Create(5, &clock_server);
  Create(2, &idle_task);

  time_delay(10);

  Assert(Kill(WhoIs("Idle")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
}
