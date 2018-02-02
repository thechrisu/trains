#include "test_messaging.h"

void receive_before_send_sender() {
  char msg[] = "Sending a message!";
  char reply[64];

  int reply_length = Send(MyParentTid(), msg, tstrlen(msg) + 1, reply, 64);
  bwprintf("Received reply: %s\n\r", reply);
  bwprintf("Reply length: %d\n\r", reply_length);
  Assert(reply_length > 0);
}

void test_messaging_receive_before_send() {
  int sender_tid;
  char msg[64];
  char reply[] = "Got your message!";

  sender_tid = Create(MyPriority() - 1, &receive_before_send_sender);
  bwprintf("Created task %d\n\r", sender_tid);

  int message_length = Receive(&sender_tid, msg, 64);
  bwprintf("Received message: %s\n\r", msg);
  bwprintf("Message length: %d\n\r", message_length);
  Assert(message_length > 0);

  Assert(Reply(sender_tid, reply, tstrlen(reply) + 1) == 0);
  bwprintf("Sent reply: %s\n\r", reply);
}
