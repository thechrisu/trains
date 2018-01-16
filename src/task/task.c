#include "task.h"


tid_t max_tid = (STACK_TOP - STACK_BOTTOM) / BYTES_PER_TASK;
tid_t next_task_id = 0;
task_descriptor *current_task;
task_descriptor *all_tasks[MAX_TASKS];

void task_init(task_descriptor *task, int priority, void (*task_main)(), task_descriptor *parent) {
  task->tid = next_task_id++;
  task->priority = priority;
  task->state = TASK_RUNNABLE;
  task->next = NULL_TASK_DESCRIPTOR;
  task->prev = NULL_TASK_DESCRIPTOR;
  task->parent = parent;
  all_tasks[task->tid] = task;

  task->tf = (trapframe *)(STACK_TOP - next_task_id * BYTES_PER_TASK - sizeof(trapframe));
  task->tf->r0 = 0xF4330000 + (task->tid << 4);
  task->tf->r1 = 0xF4330001 + (task->tid << 4);
  task->tf->r2 = 0xF4330002 + (task->tid << 4);
  task->tf->r3 = 0xF4330003 + (task->tid << 4);
  task->tf->r4 = 0xF4330004 + (task->tid << 4);
  task->tf->r5 = 0xF4330005 + (task->tid << 4);
  task->tf->r6 = 0xF4330006 + (task->tid << 4);
  task->tf->r7 = 0xF4330007 + (task->tid << 4);
  task->tf->r8 = 0xF4330008 + (task->tid << 4);
  task->tf->r9 = 0xF4330009 + (task->tid << 4);
  task->tf->r10 = 0xF433000A + (task->tid << 4);
  task->tf->fp = 0xF433000B + (task->tid << 4);
  task->tf->ip = 0xF433000C + (task->tid << 4);
#ifdef TESTING
  // Need uint64_t here, otherwise the compiler generate 'cast from pointer to smaller type' errors when compiling tests
  task->tf->sp = (uint64_t)task->tf;
  task->tf->lr = (uint64_t)(task_main); // When generating tests, we can't include the ARM asm file
  task->tf->pc = (uint64_t)task_main;
#else
  task->tf->sp = (uint32_t)task->tf;
  task->tf->lr = (uint32_t)(&sys_exit);
  task->tf->pc = (uint32_t)task_main;
#endif /* TESTING */
  // TODO set cpsr
}

void task_activate(task_descriptor *task) {
  task->state = TASK_ACTIVE;
  current_task = task;
}

void task_runnable(task_descriptor *task) {
  task->state = TASK_RUNNABLE;
}

void task_retire(task_descriptor *task, uint16_t exit_code) {
  task->state = TASK_ZOMBIE;
  task->exit_code = exit_code;
}

tid_t task_get_tid(task_descriptor *task) {
  return task->tid;
}

tid_t task_get_parent_id(task_descriptor *task) {
  if (task->parent == NULL_TASK_DESCRIPTOR) {
    return -1;
  }
  return task->parent->tid;
}
