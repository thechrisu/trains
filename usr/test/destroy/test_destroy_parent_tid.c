#include "test_destroy.h"

static void grandchild() {
  bwprintf("Grandchild - MyTid: %d, MyParentTid: %d\n\r", MyTid(), MyParentTid());
}

static void child() {
  bwprintf("Child - MyTid: %d, MyParentTid: %d\n\r", MyTid(), MyParentTid());
  Assert(Create(MyPriority() + 1, &grandchild) > 0);
}

static void dummy() {}

void test_destroy_parent_tid() {
  bwprintf("Parent - MyTid: %d\n\r", MyTid());
  Assert(Create(MyPriority() + 1, &dummy) > 0);
  Assert(Create(MyPriority() + 1, &child) > 0);
}
