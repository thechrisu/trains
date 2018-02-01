#include "syscall.h"

int syscall_create(int priority, void (*code)()) {
  if (next_task_id >= MAX_TASKS) {
    return -2;
  }
  // We do it this way instead of passing the result of register_task(),
  // because we won't use up the task's tid
  if (priority < 0 || priority > MAX_PRIORITY) {
    return -1;
  }
  task_descriptor *ret = get_next_raw_td();
#if CONTEXT_SWITCH_DEBUG
  logprintf("Got task descriptor memory\n\r");
#endif /* CONTEXT_SWITCH_DEBUG */
  task_init(ret, priority, code, get_current_task());
#if CONTEXT_SWITCH_DEBUG
  logprintf("Set up task in syscall_create, tf: %x\n\r", ret->tf);
#endif /* CONTEXT_SWITCH_DEBUG */
  int register_result = register_task(ret);
  if (register_result) {
    return register_result;
  } else {
    return task_get_tid(ret);
  }
}

int syscall_mytid() {
  if (get_current_task() == NULL_TASK_DESCRIPTOR) {
    return -1;
  } else {
    return task_get_tid(get_current_task());
  }
}

int syscall_myparent_tid() {
  if (get_current_task() == NULL_TASK_DESCRIPTOR) {
    return -2;
  } else {
    return task_get_parent_tid(get_current_task());
  }
}

void syscall_pass() {
  register_task(get_current_task());
}

void syscall_exit() {
  task_retire(get_current_task(), 0);
}

void syscall_panic() {
#ifndef TESTING
  __asm__("b panic_exit");
#endif /* TESTING */
}

void syscall_send() {
  task_descriptor *current_task = get_current_task();
#if MESSAGE_PASSING_DEBUG
  logprintf("syscall_send: sender %d, recipient %d, message %c\n\r", current_task->tid, current_task->tf->r1, *(char *)(current_task->tf->r2));
#endif
  register_t receiver_tid = (register_t)current_task->tf->r1;
  if (receiver_tid < 0 || receiver_tid >= next_task_id) {
    current_task->tf->r0 = -2;
    return;
  }
  send(current_task, (task_descriptor *)get_task_with_tid(receiver_tid));
}

void syscall_receive() {
  task_descriptor *current_task = get_current_task();
#if MESSAGE_PASSING_DEBUG
  logprintf("syscall_receive: recipient %d\n\r", current_task->tid);
#endif
  receive(current_task);
}

void syscall_reply() {
  task_descriptor *current_task = get_current_task();
#if MESSAGE_PASSING_DEBUG
  logprintf("syscall_reply: recipient %d, target %d, message %c\n\r", current_task->tid, current_task->tf->r1, *(char *)(current_task->tf->r2));
#endif
  register_t sender_tid = (register_t)current_task->tf->r1;
  if (sender_tid < 0 || sender_tid >= next_task_id) {
    current_task->tf->r0 = -2;
    return;
  }
  reply(get_task_with_tid(sender_tid), current_task);
}

int syscall_awaitevent(int event_id) {
  return event_register(event_id, get_current_task());
}

int syscall_kill(int tid) {
  task_descriptor *current_task = get_current_task();
  if (tid == current_task->tid) {
    return -2;
  }
  task_descriptor *to_kill = get_task_with_tid(tid);
  if (to_kill == NULL_TASK_DESCRIPTOR) {
    return -1;
  }
  task_set_state(to_kill, TASK_ZOMBIE);
  deregister_task(to_kill);
  event_deregister(to_kill);
  return 0;
}

void syscall_cache_enable() {
  task_descriptor *current_task = get_current_task();
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
