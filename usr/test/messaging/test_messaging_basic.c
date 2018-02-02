#include "test_messaging.h"

void basic_receiver() {
  int sender_tid;
  char msg[64];
  char reply[18] = "Got your message!";

  int message_length = Receive(&sender_tid, msg, 64);
  bwprintf("Received message: %s\n\r", msg);
  bwprintf("Message length: %d\n\r", message_length);
  Assert(message_length > 0);

  Assert(Reply(sender_tid, reply, tstrlen(reply) + 1) == 0);
  bwprintf("Sent reply: %s\n\r", reply);
}

void test_messaging_basic() {
  int receiver_tid;
  char msg[] = "Sending a message!";
  char reply[64];

  receiver_tid = Create(MyPriority() - 1, &basic_receiver);
  bwprintf("Created task %d\n\r", receiver_tid);

  int reply_length = Send(receiver_tid, msg, tstrlen(msg) + 1, reply, 64);
  bwprintf("Received reply: %s\n\r", reply);
  bwprintf("Reply length: %d\n\r", reply_length);
  Assert(reply_length > 0);
}
