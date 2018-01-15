#include "ready_queue.h"

int ready_queue_length(ready_queue *rq) {
  if (*rq == NULL_READY_QUEUE)
    return 0;

  int length = 0;
  task_descriptor *td = *rq;
  do {
    td = td->next;
    length += 1;
  } while(td != *rq);
  return length;
}

void ready_queue_push(ready_queue *rq, task_descriptor *td) {
  task_descriptor *head, *tail;

  if (ready_queue_length(rq) == 0) {
    td->prev = td;
    td->next = td;
    *rq = td;
  } else {
    head = *rq;
    tail = head->prev;
    head->prev = td;
    tail->next = td;
    td->prev = tail;
    td->next = head;
  }
}

task_descriptor *ready_queue_pop(ready_queue *rq) {
  task_descriptor *head, *new_head, *tail;

  switch (ready_queue_length(rq)) {
    case 0:
      return NULL_TASK_DESCRIPTOR;
    case 1:
      head = *rq;
      *rq = NULL_READY_QUEUE;
      return head;
    default:
      head = *rq;
      new_head = head->next;
      tail = head->prev;
      new_head->prev = tail;
      tail->next = new_head;
      *rq = new_head;
      return head;
  }
}
