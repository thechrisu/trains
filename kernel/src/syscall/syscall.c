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

void syscall_panic() {
#ifndef TESTING
  __asm__(
    "mov fp, %0\n\t"
    "mov sp, %1\n\t"
    "b panic_exit"
  : : "r" (main_fp), "r" (main_sp));
#endif /* TESTING */
}

void syscall_send() {
#if MESSAGE_PASSING_DEBUG
  bwprintf("syscall_send: sender %d, recipient %d, message %c\n\r", current_task->tid, current_task->tf->r1, *(char *)(current_task->tf->r2));
#endif
  register_t receiver_tid = (register_t)current_task->tf->r1;
  if (receiver_tid < 0 || receiver_tid >= next_task_id) {
    current_task->tf->r0 = -2;
    return;
  }
  send(current_task, &(all_tasks[receiver_tid]));
}

void syscall_receive() {
#if MESSAGE_PASSING_DEBUG
  bwprintf("syscall_receive: recipient %d\n\r", current_task->tid);
#endif
  receive(current_task);
}

void syscall_reply() {
#if MESSAGE_PASSING_DEBUG
  bwprintf("syscall_reply: recipient %d, target %d, message %c\n\r", current_task->tid, current_task->tf->r1, *(char *)(current_task->tf->r2));
#endif
  register_t sender_tid = (register_t)current_task->tf->r1;
  if (sender_tid < 0 || sender_tid >= next_task_id) {
    current_task->tf->r0 = -2;
    return;
  }
  reply(&(all_tasks[sender_tid]), current_task);
}

void syscall_cache_enable() {
  bool enable = current_task->tf->r1;
#if TESTING
  (void)enable;
#else //  0x3FFF9006
  if (enable) {
    __asm__(
	    "MRC p15, 0, r0, c1, c0, 0\n\t"
	    "MOV r1, #0x1000\n\t"
	    "ADD r1, r1, #0x06\n\t"
	    "ORR r0, r0, r1\n\t"
	    "MOV r1, #0x3F000000\n\t"
	    "ADD r1, r1, #0xFF0000\n\t"
	    "ADD r1, r1, #0x8000\n\t"
	    "BIC r0, r0, r1\n\t"
	    "MCR p15, 0, r0, c1, c0, 0\n\t");
  }
  /* TODO disbale the cache
     else {
    __asm__(
	    "MRC p15, 0, r0, c1, c0, 0\n\t"
	    "MOV r1, #0x3F000000\n\t"
	    "ADD r1, r1, #0xFF0000\n\t"
	    "ADD r1, r1, #0x9000\n\t"
	    "ADD r1, r1, #0x05\n\t"
	    "BIC r0, r0, r1\n\t"
	    "MCR p15, 0, r0, c1, c0, 0\n\t"); 
	    } */
#endif /* TESTING */  
}
