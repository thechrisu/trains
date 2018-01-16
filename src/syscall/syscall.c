#include "syscall.h"


int syscall_create(int priority, void (*code)()) {
  if (next_task_id >= MAX_TASKS) {
    return -2;
  }
  // We do it this way instead of passing the result of register_task(),
  // because we won't use up the task's tid
  if (priority < 0) {
    return -1;
  }
  task_descriptor *ret = (task_descriptor *)all_tasks[next_task_id];
  task_init(ret, priority, code, current_task);
  int register_result = register_task(ret);
  if (register_result) {
    return register_result;
  } else {
    return task_get_tid(ret);
  }
}

int syscall_mytid() {
  if (current_task == NULL_TASK_DESCRIPTOR) {
    return -1;
  } else {
    return task_get_tid(current_task);
  }
}

int syscall_myparent_tid() {
  if (current_task == NULL_TASK_DESCRIPTOR) {
    return -2;
  } else {
    return task_get_parent_tid(current_task);
  }
}

void syscall_pass() {
  // TODO Implement integration of scheduler + tasks. After that, implement this function.
}

void syscall_exit() {
#ifndef TESTING
 enter_main();
#endif /* TESTING */
}
