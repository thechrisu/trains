#include "test_destroy.h"

static void dummy() {};

void test_destroy_task_limit() {
  int tid = Create(MyPriority() - 1, &dummy);
  Assert(tid > 0);

  for (int i = 1; i <= MAX_TASKS - 4; i += 1) {
    Assert(Create(MyPriority() - 1, &dummy) > 0);
  }

  Assert(Create(MyPriority() - 1, &dummy) == -2);

  Assert(Kill(tid) == 0);
  int new_tid = Create(MyPriority() - 1, &dummy);
  Assert(new_tid = tid + MAX_TASKS);

  Assert(Create(MyPriority() - 1, &dummy) == -2);

  bwprintf("Success!\n\r");
}
