#include "test_destroy.h"

static void child() {
  Assert(RegisterAs("Child") == 0);
  Assert(WhoIs("Child") == MyTid());
}

void test_destroy_nameserver() {
  ns_tid = Create(MyPriority() + 2, &nameserver_main);

  int first_tid = Create(MyPriority() + 1, &child);
  Assert(first_tid > 0);
  Assert(WhoIs("Child") == first_tid);

  for (int i = 1; i <= 10; i += 1) {
    int tid = Create(MyPriority() + 1, &child);
    Assert(tid = first_tid + MAX_TASKS * i);
    Assert(WhoIs("Child") == tid);
  }

  bwprintf("Success!\n\r");
}
