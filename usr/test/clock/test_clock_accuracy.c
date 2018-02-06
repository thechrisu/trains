#include "test_clock.h"

int clock_server_tid;

uint32_t time() {
  message send, reply;
  send.type = MESSAGE_TIME;
  Assert(Send(clock_server_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
  Assert(reply.type == REPLY_TIME);
  return reply.msg.reply_time_ticks;
}

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
  assert_within_tick_range(elapsed_time, ticks, 1, 1);
}

void time_delay_until(uint32_t ticks) {
  message send;
  uint32_t start_time, elapsed_time;
  uint32_t ticks_at_start;
  char *s = ticks == 1 ? "" : "s";

  bwprintf("Delaying until %d tick%s from now\n\r", ticks, s);

  start_time = get_time();

  ticks_at_start = time();
  send.type = MESSAGE_DELAY_UNTIL;
  send.msg.message_delay_ticks = ticks_at_start + ticks;
  Assert(Send(clock_server_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);

  elapsed_time = get_time() - start_time;

  bwprintf("Actual time for delaying until %d tick%s from then: %d ms\n\r", ticks, s, elapsed_time);
  assert_within_tick_range(elapsed_time, ticks, 1, 1);
}

void clock_accuracy() {
  uint32_t start_time, start_milliseconds, elapsed_time, elapsed_milliseconds;

  setup_timer();

  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  clock_server_tid = Create(MyPriority() + 1, &clock_server);
  Create(2, &idle_task);

  start_milliseconds = get_time();
  start_time = time();

  Assert(0);
  time_delay(1);
  time_delay(10);
  time_delay(50);
  time_delay_until(5);
  time_delay_until(10);
  time_delay_until(50);

  elapsed_time = time() - start_time;
  elapsed_milliseconds = get_time() - start_milliseconds;

  bwprintf("Entire task took %d ticks (actual time %d milliseconds)\n\r", elapsed_time, elapsed_milliseconds);
  assert_within_tick_range(elapsed_milliseconds, elapsed_time, 2, 2);

  Assert(Kill(WhoIs("Idle")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(ns_tid) == 0);
}
