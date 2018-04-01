#include "test_sleeper.h"

void test_sleeper() {
  set_alarm(1);
  int sleeper_tid = -1;
  message empty;
  Assert(Receive(&sleeper_tid, &empty, sizeof(empty)) == 0);
  Assert(empty.type == MESSAGE_WAKEUP);
  Assert(Reply(sleeper_tid, EMPTY_MESSAGE, 0) == 0);
  bwprintf("Success.\n\r");
}
