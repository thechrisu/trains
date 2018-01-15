#include "ready_queue.h"

int ready_queue_length(ready_queue *rq) {
  if (ready_queue_is_empty(rq))
    return 0;

  int length = 0;
  task_descriptor *td = *rq;
  do {
    td = td->next;
    length += 1;
  } while(td != *rq);
  return length;
}

int ready_queue_is_empty(ready_queue *rq) {
  return *rq == NULL_READY_QUEUE;
}

/**
 * @param   rq A ready queue.
 * @returns Whether or not the queue has one element.
 */
int ready_queue_has_one_element(ready_queue *rq) {
  task_descriptor *head = *rq;
  return head->next == head;
}

void ready_queue_enqueue(ready_queue *rq, task_descriptor *td) {
  task_descriptor *head, *tail;

  if (ready_queue_is_empty(rq)) {
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

task_descriptor *ready_queue_dequeue(ready_queue *rq) {
  task_descriptor *head, *new_head, *tail;

  if (ready_queue_is_empty(rq))
    return NULL_TASK_DESCRIPTOR;

  if (ready_queue_has_one_element(rq)) {
    head = *rq;
    *rq = NULL_READY_QUEUE;
  } else {
    head = *rq;
    new_head = head->next;
    tail = head->prev;
    new_head->prev = tail;
    tail->next = new_head;
    *rq = new_head;
  }
  return head;
}
