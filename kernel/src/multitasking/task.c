#include "task.h"

extern task_descriptor *get_current_task();

uint64_t available_tids;
int the_next_generation[MAX_TASKS];
static task_descriptor all_tasks[MAX_TASKS];
static task_descriptor *send_queues[MAX_TASKS];
int num_ctx_sw = 0;
int tasks_event_blocked = 0;

tid_t get_next_available_tid() {
  return available_tids == 0 ? -1 : __builtin_clzll(available_tids);
}

task_descriptor *get_next_raw_td() {
  return &(all_tasks[get_next_available_tid()]);
}

task_descriptor *get_task_with_tid(tid_t tid) {
  if (tid >= MAX_TASKS || tid < 0) {
    return NULL_TASK_DESCRIPTOR;
  }
  return &(all_tasks[tid]);
}

task_descriptor *get_task_with_userland_tid(tid_t tid) {
  return get_task_with_tid(tid % MAX_TASKS);
}

void task_init(task_descriptor *task, int priority, void (*task_main)(), task_descriptor *parent) {
#if CONTEXT_SWITCH_DEBUG
  logprintf("Enter task_init, location of task in memory %x\n\r", task);
#endif /* CONTEXT_SWITCH_DEBUG */
  task->tid = get_next_available_tid();
  task->generation = the_next_generation[task->tid]++;
  available_tids &= ~(1 << task->tid);
#if CONTEXT_SWITCH_DEBUG
  logprintf("Was able to access task struct\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  task->priority = priority;
  task->state = TASK_RUNNABLE;
  task->next = NULL_TASK_DESCRIPTOR;
  task->prev = NULL_TASK_DESCRIPTOR;
  task->parent = parent;
  send_queues[task->tid] = NULL_TASK_DESCRIPTOR;
  task->send_queue = (task_descriptor **)&(send_queues[task->tid]);
  task->blocked_on = NOT_BLOCKED;

#ifndef TESTING
  task->tf = (trapframe *)(STACK_TOP - task->tid * BYTES_PER_TASK - sizeof(trapframe));
#if TIMERINTERRUPT_DEBUG
  logprintf("Task tf(%d): %x\n\r", task->tid, task->tf);
#endif /* TIMERINTERRUPT_DEBUG */
#else
  task->tf = (trapframe *)malloc(sizeof(trapframe)); // :(
#endif /* TESTING */
#if CONTEXT_SWITCH_DEBUG
  logprintf("task_init: Location of tf: %x\n\r", task->tf);
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
  logprintf("task_main: %x\n\r", (register_t)task_main);
#endif /* SCHEDULE_DEBUG */
}

void task_activate(task_descriptor *task) {
#if TRAPFRAME_DEBUG
  logprintf("Start of task_activate\n\r");
  print_tf(task->tf);
#endif /* TRAPFRAME_DEBUG */
  kassert((task->tf->sp > STACK_TOP - (task->tid + 2) * BYTES_PER_TASK) && (task->tf->sp <= STACK_TOP - (1 + task->tid) * BYTES_PER_TASK));
  //kassert(((task->tf->fp > STACK_TOP - (task->tid + 2) * BYTES_PER_TASK) && (task->tf->fp <= STACK_TOP - (1 + task->tid) * BYTES_PER_TASK)) || (task->tf->fp == (register_t)0xF433000B + (task->tid << 4)));
#if TIMERINTERRUPT_DEBUG
  kassert((task->tf->r7 & 0xFFFF0000) != 0xF4330000 || ((0xFFF0 & task->tf->r7) >> 4) == task->tid);
#endif /* TIMERINTERRUPT_DEBUG */
  task->state = TASK_ACTIVE;
#ifndef TESTING
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *tid_send = TID_SEND;
  volatile int16_t *tid_receive_reply = TID_RECEIVE_REPLY;
  volatile int16_t *is_receive = IS_RECEIVE;
  if (task->tid == *tid_send) {
    volatile int16_t *loc_kExit_sys_send = LOC_KEXIT_SYS_SEND;
    *loc_kExit_sys_send = get_clockticks();
    // logprintf("(%d) kExit Send\n\r", *loc_kExit_sys_send);
  }
  if (task->tid == *tid_receive_reply) {
    if (is_receive) {
    volatile int16_t *loc_kExit_sys_receive = LOC_KEXIT_SYS_RECEIVE;
      *loc_kExit_sys_receive = get_clockticks();
      // logprintf("(%d) kExit Receive\n\r", *loc_kExit_sys_receive);
    }
  }
  kassert(!(task->tid == *tid_receive_reply && task->tid == *tid_send));
#endif /* CONTEXT_SWITCH_BENCHMARK */
#if TIMERINTERRUPT_DEBUG
  if (task->tf->sp != (int)task->tf) {
    task_descriptor *current_task = get_current_task();
    logprintf("current_task: %d\n\r", current_task->tid);
    logprintf("task in leave_kernel: %d\n\r", task->tid);
    logprintf("address of trapframe: %x\n\r", (int)task->tf);
    print_tf(task->tf);
  }
#endif /* TIMERINTERRUPT_DEBUG */
  num_ctx_sw += 1;
  end_interval(KERNEL_FAKE_TID);
  start_interval();
  leave_kernel(task->tf->r0, task->tf);
#endif
#if TRAPFRAME_DEBUG
  logprintf("End of task_activate\n\r");
  print_tf(task->tf);
#endif /* TRAPFRAME_DEBUG */

#ifndef TESTING
  kassert(task == get_current_task());
#endif /* TESTING */
}

void task_set_state(task_descriptor *task, task_state state) {
  kassert(task->state != TASK_EVENT_BLOCKED || task->state != state);
  if (task->state == TASK_EVENT_BLOCKED && (state == TASK_RUNNABLE || state == TASK_ZOMBIE)) {
    tasks_event_blocked -= 1;
  } else if (state == TASK_EVENT_BLOCKED) {
    tasks_event_blocked += 1;
  }
  kassert(tasks_event_blocked >= 0 && tasks_event_blocked <= MAX_TASKS);
  task->state = state;
}

void task_retire(task_descriptor *task, int16_t exit_code) {
  task_set_state(task, TASK_ZOMBIE);
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

  available_tids |= 1 << task->tid;
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

int task_get_priority(task_descriptor *task) {
  return task->priority;
}

int task_get_userland_tid(task_descriptor *task) {
  return task->generation * MAX_TASKS + task->tid;
}

bool is_valid_userland_tid(int userland_tid) {
  tid_t kernel_tid = userland_tid % MAX_TASKS;
  return userland_tid > 0 && kernel_tid < MAX_TASKS && !(available_tids & (1 << kernel_tid));
}
