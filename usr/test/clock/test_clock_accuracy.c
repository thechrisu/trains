#include "test_clock.h"

void clock_accuracy() {
  message send, reply;
  uint32_t start_time, end_time;

  setup_timer();

  ns_tid = Create(7, &nameserver_main);
  int clock_server_tid = Create(5, &clock_server);
  Create(2, &idle_task);

  send.type = MESSAGE_DELAY;
  send.msg.message_delay_ticks = 10;

  start_time = get_time();
  Assert(Send(clock_server_tid, &send, sizeof(send), &reply, sizeof(reply)) == 0);
  end_time = get_time();
  bwprintf("Actual time for delaying 10 ticks: %d ms\n\r", end_time - start_time);

  Assert(Kill(WhoIs("Idle")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
}
