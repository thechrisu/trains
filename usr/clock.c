#include "clock.h"

/*
  Repeatedly waits for a clock event, then sends a message to the clock server.
  Created by the clock server, so sends its messages to its parent.
*/
void clock_notifier() {
  Assert(RegisterAs("ClockNotifier") == 0);
  Assert(WhoIs("ClockNotifier") == MyTid());

  int server_tid = WhoIs("ClockServer");
  Assert(server_tid > 0);

  message msg;
  msg.type = MESSAGE_CLOCK_NOTIFIER;

  while (true) {
    Assert(AwaitEvent(TIMER_INTERRUPT) == 0);
    Assert(Send(server_tid, &msg, sizeof(msg), EMPTY_MESSAGE, 0) >= 0);
  }
  Assert("ACCIDENTALLY KILLED CLOCK NOTIFIER\n\r" == "");
  Assert(0);
}

void clock_server() {
  int sender_tid;
  message received, reply;
  uint32_t ticks = 0;
  clock_wait cw, *head;
  clock_wait_queue queue;

  clock_wait_queue_init(&queue);

  Assert(RegisterAs("ClockServer") == 0);
  Assert(WhoIs("ClockServer") == MyTid());

  Create(6, &clock_notifier);

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    switch (received.type) {
      case MESSAGE_CLOCK_NOTIFIER:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        ticks += 1;

        head = clock_wait_queue_peek(&queue);
        while (head != NULL_CLOCK_WAIT && head->ticks <= ticks) {
          Assert(clock_wait_queue_dequeue(&queue, &cw) != -1);
          Assert(Reply(cw.tid, EMPTY_MESSAGE, 0) >= 0);
          head = clock_wait_queue_peek(&queue);
        }

        break;
      case MESSAGE_TIME:
        reply.type = REPLY_TIME;
        reply.msg.reply_time_ticks = ticks;
        Assert(Reply(sender_tid, &reply, sizeof(reply)) >= 0);
        break;
      case MESSAGE_DELAY:
        if (received.msg.message_delay_ticks <= 0) {
          reply.type = REPLY_CLOCK_SERVER_ERROR;
          Reply(sender_tid, &reply, sizeof(reply));
        } else {
          cw.tid = sender_tid;
          cw.ticks = ticks + received.msg.message_delay_ticks;
          Assert(clock_wait_queue_enqueue(&queue, &cw) != -1);
        }
        break;
      case MESSAGE_DELAY_UNTIL:
        if (received.msg.message_delay_until_ticks <= ticks) {
          reply.type = REPLY_CLOCK_SERVER_ERROR;
          Reply(sender_tid, &reply, sizeof(reply));
        } else {
          cw.tid = sender_tid;
          cw.ticks = received.msg.message_delay_until_ticks;
          Assert(clock_wait_queue_enqueue(&queue, &cw) != -1);
        }
        break;
      default:
        Panic();
    }
  }
}
