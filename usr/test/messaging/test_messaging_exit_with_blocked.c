#include "test_messaging.h"

void send_blocked() {
  Send(MyParentTid(), (char *)0, 0, (char *)0, 0);
  bwprintf("Send-blocked: should never reach here\n\r");
}

void receive_blocked() {
  int tid;
  Receive(&tid, (char *)0, 0);
  bwprintf("Receive-blocked: should never reach here\n\r");
}

void reply_blocked() {
  Send(MyParentTid(), (char *)0, 0, (char *)0, 0);
  bwprintf("Reply-blocked: should never reach here\n\r");
}

void test_messaging_exit_with_blocked() {
  // receive_blocked will be blocked forever on a Receive call
  Create(6, &receive_blocked);

  // reply_blocked will be blocked forever waiting for a Reply call
  int tid;
  int reply_blocked_tid = Create(6, &reply_blocked);
  Receive(&tid, (char *)0, 0);
  Assert(tid == reply_blocked_tid);

  /*
    send_blocked will run directly after this call to Create. It calls Send immediately.
    Since its parent (this task) still exists, it will block forever. If send_blocked
    had a lower priority than its parent, it would call Send after its parent had exited,
    causing it not to block.
  */
  Create(6, &send_blocked);
}
