/**
 * @file
 * @brief Generic queue implementation
 */
#ifndef QUEUE_H
#define QUEUE_H

#define CONCAT(a, b) a ## b

#define LENGTH(T)          CONCAT(T, _length)
#define IS_EMPTY(T)        CONCAT(T, _is_empty)
#define HAS_ONE_ELEMENT(T) CONCAT(T, _has_one_element)
#define ENQUEUE(T)         CONCAT(T, _enqueue)
#define DEQUEUE(T)         CONCAT(T, _dequeue)
#define REMOVE(T)          CONCAT(T, _remove)

#include "multitasking/task.h"

#endif /* QUEUE_H */

int LENGTH(QUEUE_TYPE)(QUEUE_TYPE *q) {
  if (IS_EMPTY(QUEUE_TYPE)(q))
    return 0;

  int length = 0;
  task_descriptor *td = *q;
  do {
    td = QUEUE_NEXT(td);
    length += 1;
  } while(td != *q);
  return length;
}

int IS_EMPTY(QUEUE_TYPE)(QUEUE_TYPE *q) {
  return *q == (QUEUE_TYPE)0;
}

int HAS_ONE_ELEMENT(QUEUE_TYPE)(QUEUE_TYPE *q) {
  task_descriptor *head = *q;
  return QUEUE_NEXT(head) == head;
}

void ENQUEUE(QUEUE_TYPE)(QUEUE_TYPE *q, task_descriptor *td) {
  task_descriptor *head, *tail;

  if (IS_EMPTY(QUEUE_TYPE)(q)) {
    QUEUE_PREV(td) = td;
    QUEUE_NEXT(td) = td;
    *q = td;
  } else {
    head = *q;
    tail = QUEUE_PREV(head);
    QUEUE_PREV(head) = td;
    QUEUE_NEXT(tail) = td;
    QUEUE_PREV(td) = tail;
    QUEUE_NEXT(td) = head;
  }
}

task_descriptor *DEQUEUE(QUEUE_TYPE)(QUEUE_TYPE *q) {
  task_descriptor *head, *new_head, *tail;

  if (IS_EMPTY(QUEUE_TYPE)(q))
    return NULL_TASK_DESCRIPTOR;

  if (HAS_ONE_ELEMENT(QUEUE_TYPE)(q)) {
    head = *q;
    *q = (QUEUE_TYPE)0;
  } else {
    head = *q;
    new_head = QUEUE_NEXT(head);
    tail = QUEUE_PREV(head);
    QUEUE_PREV(new_head) = tail;
    QUEUE_NEXT(tail) = new_head;
    *q = new_head;
  }
  return head;
}

void REMOVE(QUEUE_TYPE)(QUEUE_TYPE *q, task_descriptor *td) {
  if (!IS_EMPTY(QUEUE_TYPE)(q)) {
    if (HAS_ONE_ELEMENT(QUEUE_TYPE)(q)) {
      *q = (QUEUE_TYPE)0;
    } else {
      task_descriptor *prev = QUEUE_PREV(td);
      task_descriptor *next = QUEUE_NEXT(td);
      *q = next;
      QUEUE_PREV(next) = prev;
      QUEUE_NEXT(prev) = next;
    }
  }
}
