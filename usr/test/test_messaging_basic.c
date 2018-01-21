#include "test_messaging_basic.h"

void recipient() {
  int sender_tid;
  char msg[64];
  char reply[] = "Got your message!";
  Assert(Receive(&sender_tid, msg, 64) > 0);
  bwprintf("Received message: %s\n\r", msg);
  Assert(Reply(sender_tid, reply, strlen(reply)) == 0);
  bwprintf("Sent reply: %s\n\r", reply);
}

void test_messaging_basic() {
  int receiver_tid;
  char msg[] = "Sending a message!";
  char reply[64];

  receiver_tid = Create(4, &recipient);
  bwprintf("Created task %d\n\r", receiver_tid);
  int reply_length = Send(receiver_tid, msg, strlen(msg) + 1, reply, 64);
  bwprintf("Reply length: %d\n\r", reply_length);
  Assert(reply_length > 0);
  bwprintf("Got reply: %s\n\r", reply);
}
