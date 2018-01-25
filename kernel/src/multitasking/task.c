#include "task.h"

tid_t next_task_id = 1;
task_descriptor *all_tasks;
task_descriptor **send_queues;

void task_init(task_descriptor *task, int priority, void (*task_main)(), task_descriptor *parent) {
#if CONTEXT_SWITCH_DEBUG
  bwprintf("Enter task_init, location of task in memory %x\n\r", task);
#endif /* CONTEXT_SWITCH_DEBUG */
  task->tid = next_task_id;
#if CONTEXT_SWITCH_DEBUG
  bwprintf("Was able to access task struct\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  next_task_id++;
  task->priority = priority;
  task->state = TASK_RUNNABLE;
  task->next = NULL_TASK_DESCRIPTOR;
  task->prev = NULL_TASK_DESCRIPTOR;
  task->parent = parent;
  send_queues[task->tid] = NULL_TASK_DESCRIPTOR;
  task->send_queue = (task_descriptor **)&(send_queues[task->tid]);

#ifndef TESTING
  task->tf = (trapframe *)(STACK_TOP - next_task_id * BYTES_PER_TASK - sizeof(trapframe));
#else
  task->tf = (trapframe *)malloc(sizeof(trapframe)); // :(
#endif /* TESTING */
#if CONTEXT_SWITCH_DEBUG
  bwprintf("task_init: Location of tf: %x\n\r", task->tf);
#endif /* CONTEXT_SWITCH_DEBUG */
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
  task->tf->sp = (register_t)task->tf;
#ifdef TESTING
  // Need uint64_t here, otherwise the compiler generate 'cast from pointer to smaller type' errors when compiling tests
  task->tf->lr = (register_t)(task_main); // When generating tests, we can't include the ARM asm file
  task->tf->pc = (register_t)task_main;
#else
  task->tf->sp = (register_t)task->tf;
  task->tf->lr = (register_t)(&sys_exit);
  task->tf->pc = 0xF433000D + (task->tid << 4);
#endif /* TESTING */
  task->tf->k_lr = (register_t)task_main;
  task->tf->psr = 0x10;

#if SCHEDULE_DEBUG
  bwprintf("task_main: %x\n\r", (register_t)task_main);
#endif /* SCHEDULE_DEBUG */
}

void task_activate(task_descriptor *task) {
#if TRAPFRAME_DEBUG
  bwprintf("Start of task_activate\n\r");
  print_tf(task->tf);
#endif /* TRAPFRAME_DEBUG */
  task->state = TASK_ACTIVE;
  current_task = task;
#ifndef TESTING
#if CONTEXT_SWITCH_BENCHMARK
  if (task->tid == *tid_send) {
    *loc_kExit_sys_send = get_clockticks();
    // bwprintf("(%d) kExit Send\n\r", *loc_kExit_sys_send);
  }
  if (task->tid == *tid_receive_reply) {
    if (is_receive) {
      *loc_kExit_sys_receive = get_clockticks();
      // bwprintf("(%d) kExit Receive\n\r", *loc_kExit_sys_receive);
    }
  }
  kassert(!(task->tid == *tid_receive_reply && task->tid == *tid_send));
#endif /* CONTEXT_SWITCH_BENCHMARK */
  leave_kernel(task->tf->r0, task->tf);
#endif
#if TRAPFRAME_DEBUG
  bwprintf("End of task_activate\n\r");
  print_tf(task->tf);
#endif /* TRAPFRAME_DEBUG */
}

void task_set_state(task_descriptor *task, task_state state) {
  task->state = state;
}

void task_retire(task_descriptor *task, int16_t exit_code) {
  task->state = TASK_ZOMBIE;
  task->exit_code = exit_code;
  task->tf->r0 = 0x745C0000 + task->tid;
  task->tf->r1 = 0x745C0000 + task->tid;
  task->tf->r2 = 0x745C0000 + task->tid;
  task->tf->r3 = 0x745C0000 + task->tid;
  task->tf->r4 = 0x745C0000 + task->tid;
  task->tf->r5 = 0x745C0000 + task->tid;
  task->tf->r6 = 0x745C0000 + task->tid;
  task->tf->r7 = 0x745C0000 + task->tid;
  task->tf->r8 = 0x745C0000 + task->tid;
  task->tf->r9 = 0x745C0000 + task->tid;
  task->tf->r10 = 0x745C0000 + task->tid;
  task->tf->fp = 0x745C0000 + task->tid;
  task->tf->ip = 0x745C0000 + task->tid;
  task->tf->sp = 0x745C0000 + task->tid;
  task->tf->lr = 0x745C0000 + task->tid;
  task->tf->sp = 0x745C0000 + task->tid;
  task->tf = NULL_TRAPFRAME;
}

tid_t task_get_tid(task_descriptor *task) {
  return task->tid;
}

tid_t task_get_parent_tid(task_descriptor *task) {
  if (task->parent == NULL_TASK_DESCRIPTOR) {
    return -1;
  }
  return task->parent->tid;
}
