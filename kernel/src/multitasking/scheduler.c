#include "scheduler.h"

/**
 * Sets a bit in the given scheduler's nonempty ready queue bitmap.
 *
 * @param s        A scheduler.
 * @param priority The bit to modify.
 * @param set      Whether to set or unset the bit.
 */
void scheduler_set_nonempty(scheduler *s, int priority, bool set);

int scheduler_init(scheduler *s, int max_priority, ready_queue *queues) {
  if (max_priority < 0 || max_priority > 63)
    return -1;

  s->max_priority = max_priority;
  s->nonempty_hi = 0;
  s->nonempty_lo = 0;

  s->queues = queues;
  for (int i = 0; i <= max_priority; i += 1)
    queues[i] = NULL_READY_QUEUE;

  return 0;
}

int scheduler_register(scheduler *s, task_descriptor *td) {
  int priority = td->priority;
  if (priority < 0 || priority > s->max_priority)
    return -1;
  ready_queue_enqueue(&(s->queues[priority]), td);
  scheduler_set_nonempty(s, priority, true);
  return 0;
}

task_descriptor *scheduler_next_task(scheduler *s) {
  int highest_priority;
  if (s->nonempty_hi != 0) {
    highest_priority = 63 - __builtin_clz(s->nonempty_hi);
  } else if (s->nonempty_lo != 0) {
    highest_priority = 31 - __builtin_clz(s->nonempty_lo);
  } else {
    return NULL_TASK_DESCRIPTOR;
  }
  ready_queue *rq = &(s->queues[highest_priority]);
  task_descriptor *result = ready_queue_dequeue(rq);
  if (ready_queue_is_empty(rq))
    scheduler_set_nonempty(s, highest_priority, false);
  return result;
}

int scheduler_deregister(scheduler *s, task_descriptor *td) {
  int priority = td->priority;
  ready_queue *rq = &(s->queues[priority]);
  ready_queue_remove(rq, td);
  if (ready_queue_is_empty(rq)) {
    scheduler_set_nonempty(s, priority, false);
  }
  return 0;
}

void scheduler_set_nonempty(scheduler *s, int priority, bool set) {
  if (set) {
    if (priority > 31)
      s->nonempty_hi |= 1 << (priority - 32);
    else
      s->nonempty_lo |= 1 << priority;
  } else {
    if (priority > 31)
      s->nonempty_hi &= ~(1 << (priority - 32));
    else
      s->nonempty_lo &= ~(1 << priority);
  }
}
