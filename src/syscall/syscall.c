#include "syscall.h"

extern unsigned int main_fp;
extern unsigned int main_sp;

int syscall_create(int priority, void (*code)()) {
  if (next_task_id >= MAX_TASKS) {
    return -2;
  }
  // We do it this way instead of passing the result of register_task(),
  // because we won't use up the task's tid
  if (priority < 0 || priority > MAX_PRIORITY) {
    return -1;
  }
  task_descriptor *ret = &(all_tasks[next_task_id]);
#if CONTEXT_SWITCH_DEBUG
  bwprintf("Got task descriptor memory\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  task_init(ret, priority, code, current_task);
#if CONTEXT_SWITCH_DEBUG
  bwprintf("Set up task in syscall_create\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
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
  register_task(current_task);
}

void syscall_exit() {
  task_retire(current_task, 0);
}

void syscall_abort() {
  __asm__(
    "mov fp, %0\n\t"
    "mov sp, %1\n\t"
    "b abort_exit"
  : : "r" (main_fp), "r" (main_sp));
}
