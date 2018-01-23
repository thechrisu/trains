#include "test_messaging.h"

void same_priority_receiver() {
  int sender_tid;
  char msg[64];
  char reply[] = "Got your message!";

  Assert(Receive(&sender_tid, msg, 64) >= 0);
  Assert(Reply(sender_tid, reply, strlen(reply) + 1) == 0);
  bwprintf("Receiver should run second\n\r");
}

void test_messaging_same_priority() {
  int receiver_tid;
  char msg[] = "Sending a message!";
  char reply[64];

  receiver_tid = Create(5, &same_priority_receiver);
  Assert(Send(receiver_tid, msg, strlen(msg) + 1, reply, 64) >= 0);
  bwprintf("Sender should run first\n\r");
}
