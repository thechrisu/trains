#include "syscall.h"

int syscall_create(int priority, void (*code)()) {
  if (get_next_available_tid() == -1) {
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
    return task_get_userland_tid(ret);
  }
}

int syscall_mytid() {
  if (get_current_task() == NULL_TASK_DESCRIPTOR) {
    return -1;
  } else {
    return task_get_userland_tid(get_current_task());
  }
}

int syscall_myparent_tid() {
  if (get_current_task() == NULL_TASK_DESCRIPTOR) {
    return -2;
  } else {
    return task_get_userland_parent_tid(get_current_task());
  }
}

void syscall_pass() {
  register_task(get_current_task());
}

/**
 * Clears the send queue of a task, making all the tasks on the queue runnable and
 * registering them with the scheduler.
 *
 * @param td A task descriptor.
 */
void clear_send_queue(task_descriptor *td) {
  send_queue *q = td->send_queue;
  while (!send_queue_is_empty(q)) {
    task_descriptor *sender = send_queue_dequeue(q);
    sender->tf->r0 = -2;
    task_set_state(sender, TASK_RUNNABLE);
    register_task(sender);
  }
}

void syscall_exit() {
  task_descriptor *td = get_current_task();
  task_retire(td, 0);
  clear_send_queue(td);
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
  if (!is_valid_userland_tid(receiver_tid)) {
    current_task->tf->r0 = -2;
    return;
  }
  send(current_task, (task_descriptor *)get_task_with_userland_tid(receiver_tid));
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
  if (!is_valid_userland_tid(sender_tid)) {
    current_task->tf->r0 = -2;
    return;
  }
  reply(get_task_with_userland_tid(sender_tid), current_task);
}

int syscall_awaitevent(int event_id) {
  return event_register(event_id, get_current_task());
}

extern trapframe *handle_vic_event(task_descriptor *current_task, int highest_prio_event);

int syscall_kill(int tid) {
  task_descriptor *current_task = get_current_task();
  if (tid == task_get_userland_tid(current_task)) {
    return -2;
  }
  task_descriptor *to_kill = get_task_with_userland_tid(tid);
  if (to_kill == NULL_TASK_DESCRIPTOR) {
    return -1;
  }
  int blocked_on = to_kill->blocked_on;
  event_deregister(to_kill);
#ifndef TESTING
  if (blocked_on != NOT_BLOCKED) {
    trapframe * junk = handle_vic_event(to_kill, blocked_on);
    (void*)junk;
  }
#endif /* TESTING */
  deregister_task(to_kill);
  task_retire(to_kill, 0);
  clear_send_queue(to_kill);
  return 0;
}

int syscall_mypriority() {
  if (get_current_task() == NULL_TASK_DESCRIPTOR) {
    return -1;
  } else {
    return task_get_priority(get_current_task());
  }
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
