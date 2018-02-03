#include "test_messaging.h"

void fifo_sender() {
  int parent_tid = MyParentTid();
  char msg[2];

  msg[0] = 'A' + MyTid() - 3;
  msg[1] = '\0';

  Assert(Send(parent_tid, msg, tstrlen(msg) + 1, (char *)0, 0) == 0);
}

void test_messaging_fifo_send() {
  int sender_tid;
  char msg[2];

  for (int i = 0; i < 4; i += 1) {
    Create(MyPriority() - 1, &fifo_sender);
  }

  for (int i = 0; i < 4; i += 1) {
    Assert(Receive(&sender_tid, msg, 2) == 2);
    Reply(sender_tid, (char *)0, 0);
    bwprintf("Received \"%s\" from task %d\n\r", msg, sender_tid);
  }
}
