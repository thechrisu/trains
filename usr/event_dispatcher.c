#include "event_dispatcher.h"

void event_dispatcher() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid,
                 &received, sizeof(received)) == sizeof(received));
  Assert(sender_tid == MyParentTid());
  Assert(received.type == MESSAGE_INIT);

  int tid = received.msg.edparams.tid;

  message send;
  send.type = MESSAGE_EVENT;
  tmemcpy(&send.msg.event, &received.msg.edparams.event, sizeof(send.msg.event));

  Assert(Send(tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}
