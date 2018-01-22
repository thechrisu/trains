#include "../include/common/codes.h"
#include "myio.h"

void k1_other_user_task() {
  bwprintf("In other task: MyTid(): %d, MyParentTid(): %d\n\r", MyTid(), MyParentTid());
  Pass();
  bwprintf("In other task: MyTid(): %d, MyParentTid(): %d\n\r", MyTid(), MyParentTid());
  Exit();
}

int k1_first_user_task() {
  int new_task_id;
  new_task_id = Create(4, &k1_other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  new_task_id = Create(4, &k1_other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  new_task_id = Create(6, &k1_other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  new_task_id = Create(6, &k1_other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  bwprintf("FirstUserTask: exiting\n\r");
  Exit();
}
