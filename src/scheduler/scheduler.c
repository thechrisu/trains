#include "scheduler.h"

int scheduler_init(scheduler *s, int max_priority, ready_queue *queues) {
  (void)s;
  (void)max_priority;
  (void)queues;
}

int scheduler_register(scheduler *s, task_descriptor *td) {
  (void)s;
  (void)td;
  return 0;
}

task_descriptor *scheduler_next_task(scheduler *s) {
  (void)s;
  return (task_descriptor *)0;
}
