/**
 * @file
 * @brief Generic priority queue implementation
 */
#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "tstdlib.h"
#include "myio.h"

#define CONCAT(a, b) a ## b

#define INIT(T)            CONCAT(T, _init)
#define SWAP(T)            CONCAT(T, _swap)
#define SINK(T)            CONCAT(T, _sink)
#define HEAPIFY(T)         CONCAT(T, _heapify)
#define ENQUEUE(T)         CONCAT(T, _enqueue)
#define DEQUEUE(T)         CONCAT(T, _dequeue)
#define PEEK(T)            CONCAT(T, _peek)

#define AT(q, i)           (&(q->elts[i]))
#define LEFT_CHILD(i)      (2 * i + 1)
#define RIGHT_CHILD(i)     (2 * i + 2)
#define PARENT(i)          ((i - 1) / 2)
#define IN_BOUNDS(q, i)    (i < q->size)

#endif /* PRIORITY_QUEUE_H */

void INIT(QUEUE_TYPE)(QUEUE_TYPE *q) {
  q->size = 0;
}

void SWAP(QUEUE_TYPE)(QUEUE_TYPE *q, uint32_t i1, uint32_t i2) {
  ELEMENT_TYPE e;
  ELEMENT_COPY(AT(q, i1), &e);
  ELEMENT_COPY(AT(q, i2), AT(q, i1));
  ELEMENT_COPY(&e, AT(q, i2));
}

void SINK(QUEUE_TYPE)(QUEUE_TYPE *q, uint32_t start_index) {
  uint32_t index = start_index, smallest, left, right;

  while (true) {
    smallest = index;

    left = LEFT_CHILD(smallest);
    right = RIGHT_CHILD(smallest);

    if (IN_BOUNDS(q, left) && ELEMENT_VALUE(AT(q, left)) < ELEMENT_VALUE(AT(q, smallest))) {
      smallest = left;
    }
    if (IN_BOUNDS(q, right) && ELEMENT_VALUE(AT(q, right)) < ELEMENT_VALUE(AT(q, smallest))) {
      smallest = right;
    }

    if (smallest == index) {
      break;
    } else {
      SWAP(QUEUE_TYPE)(q, index, smallest);
      index = smallest;
    }
  }
}

void HEAPIFY(QUEUE_TYPE)(QUEUE_TYPE *q) {
  for (uint32_t i = q->size; i > 0; i -= 1) {
    SINK(QUEUE_TYPE)(q, i - 1);
  }
}

int ENQUEUE(QUEUE_TYPE)(QUEUE_TYPE *q, ELEMENT_TYPE *e) {
  if (q->size == QUEUE_SIZE) {
    return -1;
  }

  ELEMENT_COPY(e, AT(q, q->size));
  q->size += 1;

  uint32_t index = q->size - 1;
  uint32_t parent_index = PARENT(index);

  while (IN_BOUNDS(q, index) && IN_BOUNDS(q, parent_index) && ELEMENT_VALUE(AT(q, index)) < ELEMENT_VALUE(AT(q, parent_index))) {
    SWAP(QUEUE_TYPE)(q, index, parent_index);
    index = parent_index;
    parent_index = PARENT(index);
  }

  return 0;
}

int DEQUEUE(QUEUE_TYPE)(QUEUE_TYPE *q, ELEMENT_TYPE *e) {
  if (q->size == 0) {
    return -1;
  }

  SWAP(QUEUE_TYPE)(q, 0, q->size - 1);
  ELEMENT_COPY(AT(q, q->size - 1), e);
  q->size -= 1;
  SINK(QUEUE_TYPE)(q, 0);

  return 0;
}

ELEMENT_TYPE *PEEK(QUEUE_TYPE)(QUEUE_TYPE *q) {
  return q->size == 0 ? (ELEMENT_TYPE *)0 : AT(q, 0);
}
