#include "test_messaging.h"

void leaf() {
  int my_tid = MyTid();

  char msg[2];
  msg[0] = 'A' + (my_tid % 64) - 4;
  msg[1] = '\0';

  bwprintf("Leaf %d: %s\n\r", my_tid, msg);

  Send(MyParentTid(), msg, 1, (char *)0, 0);
}

void internal_node() {
  int tid;
  char msg[5];

  for (int i = 0; i < 4; i += 1) {
    Create(MyPriority() + 2, &leaf);
  }

  for (int i = 0; i < 4; i += 1) {
    Receive(&tid, msg + i, 1);
    Reply(tid, (char *)0, 0);
  }

  msg[4] = '\0';

  bwprintf("Internal node %d: %s\n\r", MyTid(), msg);

  Send(MyParentTid(), msg, 4, (char *)0, 0);
}

void test_messaging_tree() {
  int tid;
  char msg[9];

  for (int i = 0; i < 2; i += 1) {
    Create(MyPriority() + 1, &internal_node);
  }

  for (int i = 0; i < 2; i += 1) {
    Receive(&tid, msg + 4 * i, 4);
    Reply(tid, (char *)0, 0);
  }

  msg[8] = '\0';

  bwprintf("Root: %s\n\r", msg);
}
