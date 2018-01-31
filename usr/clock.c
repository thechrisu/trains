#include "clock.h"

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

  while (true) {
    Receive(&sender_tid, &received, sizeof(received));

    switch (received.type) {
      case MESSAGE_CLOCK_NOTIFIER:
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        ticks += 1;
        // TODO reply to registered tasks whose time has come
        break;
      case MESSAGE_TIME:
        reply.type = REPLY_TIME;
        reply.msg.message_time_ticks = ticks;
        Reply(sender_tid, &reply, sizeof(reply));
        break;
      case MESSAGE_DELAY:
        // TODO register task with time received.msg.message_delay_ticks + ticks
        break;
      case MESSAGE_DELAY_UNTIL:
        // TODO register task with time received.msg.message_delay_until_ticks
        break;
      default:
        Panic();
    }
  }
}
