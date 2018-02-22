#include "test_destroy.h"

static void child() {
  Assert(Send(MyParentTid(), EMPTY_MESSAGE, 0, EMPTY_MESSAGE, 0) == -2);
  bwprintf("Child with tid %d exiting\n\r", MyTid());
}

void test_destroy_send_queue() {
  for (int i = 0; i < 20; i += 1) {
    int child_tid = Create(MyPriority() + 1, &child);
    Assert(child_tid > 0);
    if (i % 2 == 0) Assert(Kill(child_tid) == 0);
  }
  bwprintf("Parent exiting\n\r");
}
