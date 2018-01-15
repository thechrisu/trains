#include "ready_queue.h"

int ready_queue_length(ready_queue *rq) {
  (void)rq;
  return 0;
}

void ready_queue_push(ready_queue *rq, task_descriptor *td) {
  (void)rq;
  (void)td;
}

task_descriptor *ready_queue_pop(ready_queue *rq) {
  (void)rq;
  return 0;
}
