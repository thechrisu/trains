#include "syscall.h"


void syscall_create(int priority, void (*code)()) {
  if (next_task_id >= MAX_TASKS)
    ; // TODO return error
  task_descriptor *ret = (task_descriptor *)all_tasks[next_task_id];
  task_init(ret, priority, code, current_task);
}

int syscall_mytid() {
  return task_get_tid(current_task);
}

int syscall_myparent_tid() {
  return task_get_parent_tid(current_task);
}

void syscall_pass() {
  register_task(current_task);
#ifndef TESTING
  enter_main(); // will call schedule()
#endif /* TESTING */
}

void syscall_exit() {
#ifndef TESTING
 enter_main();
#endif /* TESTING */
}
