#include "test_clock.h"

void clock_errors() {
  bwprintf("Creating nameserver\n\r");
  ns_tid = Create(7, &nameserver_main);
  bwprintf("Creating idle task\n\r");
  Create(2, &idle_task);

  message send, reply;

  bwprintf("Creating clock server\n\r");
  int clock_server_tid = Create(5, &clock_server);

  bwprintf("Getting time\n\r");
  send.type = MESSAGE_TIME;
  Send(clock_server_tid, &send, sizeof(send), &reply, sizeof(reply));

  int ticks_at_start = reply.msg.reply_time_ticks;
  bwprintf("Ticks at start: %d\n\r", ticks_at_start);

  send.type = MESSAGE_DELAY;
  send.msg.message_delay_ticks = -50;
  tmemset(&reply, 'Z', sizeof(reply));
  Send(clock_server_tid, &send, sizeof(send), &reply, sizeof(reply));
  bwprintf("Response type on negative delay: %d\n\r", reply.type);

  send.type = MESSAGE_DELAY;
  send.msg.message_delay_ticks = 0;
  tmemset(&reply, 'Z', sizeof(reply));
  Send(clock_server_tid, &send, sizeof(send), &reply, sizeof(reply));
  bwprintf("Response type on zero delay: %d\n\r", reply.type);

  send.type = MESSAGE_DELAY_UNTIL;
  send.msg.message_delay_ticks = ticks_at_start;
  tmemset(&reply, 'Z', sizeof(reply));
  Send(clock_server_tid, &send, sizeof(send), &reply, sizeof(reply));
  bwprintf("Response type on delay to time in past/present: %d\n\r", reply.type);

  Kill(ns_tid);
  Kill(WhoIs("ClockNotifier"));
  Kill(WhoIs("Idle"));
}
