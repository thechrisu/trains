#include "test_messaging_truncation.h"

void receiver() {
  int sender_tid;
  int length;
  char msg[5];
  char reply[] = "Got your message!";

  length = Receive(&sender_tid, msg, 5);
  bwprintf("Message length: %d\n\r", length);

  length = Reply(sender_tid, reply, strlen(reply) + 1);
  bwprintf("Reply status (receiver): %d\n\r", length);
}

void test_messaging_truncation() {
  int receiver_tid;
  int length;
  char msg[] = "Sending a message!";
  char reply[5];

  receiver_tid = Create(4, &receiver);
  bwprintf("Created task %d\n\r", receiver_tid);

  length = Send(receiver_tid, msg, strlen(msg) + 1, reply, 5);
  bwprintf("Reply length (sender): %d\n\r", length);
}
