#include "switcher.h"

void switcher() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
  Assert(sender_tid == MyParentTid());
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
  Assert(received.type == MESSAGE_SWITCH);

  int clock_server_tid = received.msg.switch_params.clock_server_tid;
  int tx_server_tid = received.msg.switch_params.tx_server_tid;

  char buf[2];
  buf[0] = (char)(received.msg.switch_params.curved ? 0x22 : 0x21);
  buf[1] = (char)received.msg.switch_params.switch_num;

  PutBytes(tx_server_tid, buf, 2);
  Delay(clock_server_tid, 15);
  Putc(tx_server_tid, TRAIN, (char)0x20);
}
