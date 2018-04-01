#include "clock.h"

void clock_server() {
  int sender_tid;
  message received, reply;
  int32_t ticks = 0;
  clock_wait cw, *head;
  clock_wait_queue queue;

  clock_wait_queue_init(&queue);

  Assert(RegisterAs("ClockServer") == 0);
  Assert(WhoIs("ClockServer") == MyTid());

  Assert(Create(MyPriority() + 1, &clock_notifier) >= 0);

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    switch (received.type) {
      case MESSAGE_NOTIFIER:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        ticks += 1;

        head = clock_wait_queue_peek(&queue);
        while (head != NULL_CLOCK_WAIT && head->ticks <= ticks) {
          Assert(clock_wait_queue_dequeue(&queue, &cw) != -1);
          reply.type = REPLY_CLOCK_SERVER_OK;
          // Won't accidentally send to the wrong task, because task ids are unique.
          int r = Reply(cw.tid, &reply, sizeof(reply));
          Assert(r == 0 || r == -2);
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
        Assert(0);
    }
  }

  Assert(0);
}
