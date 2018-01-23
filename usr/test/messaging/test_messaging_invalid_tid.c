#include "test_messaging.h"

void test_messaging_invalid_tid() {
  int error_code;
  int next_tid = MyTid() + 1;

  error_code = Send(-1, (char *)0, 0, (char *)0, 0);
  bwprintf("Send task ID -1: %d\n\r", error_code);

  error_code = Send(next_tid, (char *)0, 0, (char *)0, 0);
  bwprintf("Send task ID %d: %d\n\r", next_tid, error_code);

  error_code = Reply(-1, (char *)0, 0);
  bwprintf("Reply task ID -1: %d\n\r", error_code);

  error_code = Reply(next_tid, (char *)0, 0);
  bwprintf("Reply task ID %d: %d\n\r", next_tid, error_code);
}
