#include "send_queue.h"

int send_queue_length(send_queue *rq) {
  if (send_queue_is_empty(rq))
    return 0;

  int length = 0;
  task_descriptor *td = *rq;
  do {
    td = td->nextmsg;
    length += 1;
  } while(td != *rq);
  return length;
}

int send_queue_is_empty(send_queue *rq) {
  return *rq == NULL_SEND_QUEUE;
}

/**
 * @param   rq A send queue.
 * @returns Whether or not the queue has one element.
 */
int send_queue_has_one_element(send_queue *rq) {
  task_descriptor *head = *rq;
  return head->nextmsg == head;
}

void send_queue_enqueue(send_queue *rq, task_descriptor *td) {
  task_descriptor *head, *tail;

  if (send_queue_is_empty(rq)) {
    td->prevmsg = td;
    td->nextmsg = td;
    *rq = td;
  } else {
    head = *rq;
    tail = head->prevmsg;
    head->prevmsg = td;
    tail->nextmsg = td;
    td->prevmsg = tail;
    td->nextmsg = head;
  }
}

task_descriptor *send_queue_dequeue(send_queue *rq) {
  task_descriptor *head, *new_head, *tail;

  if (send_queue_is_empty(rq))
    return NULL_TASK_DESCRIPTOR;

  if (send_queue_has_one_element(rq)) {
    head = *rq;
    *rq = NULL_SEND_QUEUE;
  } else {
    head = *rq;
    new_head = head->nextmsg;
    tail = head->prevmsg;
    new_head->prevmsg = tail;
    tail->nextmsg = new_head;
    *rq = new_head;
  }
  return head;
}
