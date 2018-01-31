#include "clock.h"

/*
  Repeatedly waits for a clock event, then sends a message to the clock server.
  Created by the clock server, so sends its messages to its parent.
*/
void clock_notifier() {
  int server_tid = MyParentTid();

  message msg = {
    .type = MESSAGE_CLOCK_NOTIFIER
  };

  while (true) {
    AwaitEvent(TIMER_INTERRUPT);
    Send(MyParentTid, msg, sizeof(msg), EMPTY_MESSAGE, 0);
  }
}

void clock_server() {
  int sender_tid;
  message received, reply;
  int ticks = 0;
  clock_wait cw, *head;
  clock_wait_queue queue;

  clock_wait_queue_init(queue);

  Create(6, &clock_notifier);

  while (true) {
    Receive(&sender_tid, &received, sizeof(received));

    switch (received.type) {
      case MESSAGE_CLOCK_NOTIFIER:
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        ticks += 1;

        head = clock_wait_queue_peek(queue);
        while (head != NULL_CLOCK_WAIT) {
          Assert(clock_wait_queue_dequeue(queue, cw) != -1);
          Reply(cw.tid, EMPTY_MESSAGE, 0);
          head = clock_wait_queue_peek(queue);
        }

        break;
      case MESSAGE_TIME:
        reply.type = REPLY_TIME;
        reply.msg.message_time_ticks = ticks;
        Reply(sender_tid, &reply, sizeof(reply));
        break;
      case MESSAGE_DELAY:
        cw.tid = sender_tid;
        cw.ticks = ticks + received.msg.message_delay_ticks;
        Assert(clock_wait_queue_enqueue(queue, cw) != -1);
        break;
      case MESSAGE_DELAY_UNTIL:
        cw.tid = sender_tid;
        cw.ticks = received.msg.message_delay_until_ticks;
        Assert(clock_wait_queue_enqueue(queue, cw) != -1);
        break;
      default:
        Panic();
    }
  }
}
