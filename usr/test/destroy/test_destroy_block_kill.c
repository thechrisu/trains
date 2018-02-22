#include "test_destroy.h"

static void child() {
  Assert(Send(MyParentTid(), EMPTY_MESSAGE, 0, EMPTY_MESSAGE, 0) == -2);
  bwprintf("Should never reach this point\n\r");
}

void test_destroy_block_kill() {
  int child_tid = Create(MyPriority() + 1, &child);
  Assert(child_tid > 0);
  Assert(Kill(child_tid) == 0);
  bwprintf("Success!\n\r");
}
