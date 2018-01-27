#include "test_messaging.h"

void sequence_child() {
  int parent_tid;
  int length;
  char sent_msg[] = "Message to parent";
  char received_msg[64];
  char sent_reply[] = "Reply to parent";
  char received_reply[64];

  for (int i = 0; i < 4; i += 1) {
    length = Receive(&parent_tid, received_msg, 64);
    bwprintf("Received message from parent: %s\n\r", received_msg);
    Assert(length > 0);

    length = Reply(parent_tid, sent_reply, tstrlen(sent_reply) + 1);
    bwprintf("Sent reply to parent: %s\n\r", sent_reply);
    Assert(length == 0);

    length = Send(parent_tid, sent_msg, tstrlen(sent_msg) + 1, received_reply, 64);
    bwprintf("Received reply from parent: %s\n\r", received_reply);
    Assert(length > 0);
  }
}

void test_messaging_sequence() {
  int child_tid;
  int length;
  char sent_msg[] = "Message to child";
  char received_msg[64];
  char sent_reply[] = "Reply to child";
  char received_reply[64];

  child_tid = Create(4, &sequence_child);
  bwprintf("Created task %d\n\r", child_tid);

  for (int i = 0; i < 4; i += 1) {
    length = Send(child_tid, sent_msg, tstrlen(sent_msg) + 1, received_reply, 64);
    bwprintf("Received reply from child: %s\n\r", received_reply);
    Assert(length > 0);

    length = Receive(&child_tid, received_msg, 64);
    bwprintf("Received message from child: %s\n\r", received_msg);
    Assert(length > 0);

    length = Reply(child_tid, sent_reply, tstrlen(sent_reply) + 1);
    bwprintf("Sent reply to child: %s\n\r", sent_reply);
    Assert(length == 0);
  }
}
