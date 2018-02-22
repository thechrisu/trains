#include "test_destroy.h"

static void dummy() {};

void test_destroy_many_tasks() {
  int first_tid = Create(MyPriority() + 1, &dummy);

  for (int i = 1; i <= 999; i += 1) {
    int tid = Create(MyPriority() + 1, &dummy);
    Assert(tid == first_tid + MAX_TASKS * i);
  }

  for (int i = 0; i <= 999; i += 1) {
    int tid = Create(MyPriority() + 1, &dummy);
    Assert(tid == first_tid + 1 + MAX_TASKS * i);
  }

  bwprintf("Success!\n\r");
}
