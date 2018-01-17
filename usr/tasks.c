#include "../include/common/codes.h"
#include "myio.h"

void other_user_task() {
  bwprintf("In other task: MyTid(): %d, MyParentTid(): %d\n\r", MyTid(), MyParentTid());
  Pass();
  bwprintf("In other task: MyTid(): %d, MyParentTid(): %d\n\r", MyTid(), MyParentTid());
  Exit();
}

int first_user_task() {
  int new_task_id;
  new_task_id = Create(4, &other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  new_task_id = Create(4, &other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  new_task_id = Create(6, &other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);
  new_task_id = Create(6, &other_user_task);
  bwprintf("Created: %d.\n\r", new_task_id);

  bwprintf("FirstUserTask: exiting\n\r");
  Exit();
}
