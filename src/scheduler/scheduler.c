#include "scheduler.h"

int scheduler_init(scheduler *s, int max_priority, ready_queue *queues) {
  if (max_priority < 0)
    return -1;

  s->max_priority = max_priority;
  s->queues = queues;
  for (int i = 0; i <= max_priority; i += 1)
    queues[i] = NULL_READY_QUEUE;

  return 0;
}

int scheduler_register(scheduler *s, task_descriptor *td) {
  int priority = td->priority;
  if (priority < 0 || priority > s->max_priority)
    return -1;
  ready_queue_push(&(s->queues[priority]), td);
  return 0;
}

task_descriptor *scheduler_next_task(scheduler *s) {
  for (int i = s->max_priority; i >= 0; i -= 1) {
    ready_queue *rq = &(s->queues[i]);
    if (!ready_queue_is_empty(rq))
      return ready_queue_pop(rq);
  }
  return NULL_TASK_DESCRIPTOR;
}
