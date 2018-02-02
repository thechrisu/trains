#include "test_messaging.h"

int target_tid;

void reply_to_zombie_target() {
  int error_code = Reply(MyParentTid(), (char *)0, 0);
  bwprintf("Error code: %d\n\r", error_code);
}

void test_messaging_reply_target_zombie() {
  target_tid = Create(MyPriority() - 1, &reply_to_zombie_target);
  bwprintf("Exiting...\n\r");
}
