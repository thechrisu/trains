/**
 * @file
 * @brief Implementation of ring buffers
 */
#ifndef TRAINS_BUFFER_H
#define TRAINS_BUFFER_H

#include "tstdlib.h"

#define CONCAT(a, b) a ## b

#define INIT(T)                CONCAT(T, _init)
#define PUT(T)                 CONCAT(T, _put)
#define PUT_SAFE(T)            CONCAT(T, _put_safe)
#define PUT_REPLACE(T)         CONCAT(T, _put_replace)
#define REMOVE(T)              CONCAT(T, _remove)
#define GET(T)                 CONCAT(T, _get)
#define EMPTY(T)               CONCAT(T, _empty)

// Would have implemented this using _Generic, but we are on C99.
#define LENGTH(T)              CONCAT(T, _length)
#define IS_FULL(T)             CONCAT(T, _is_full)
#define IS_EMPTY(T)            CONCAT(T, _is_empty)

#define ITER_PREV_STARTI(T)    CONCAT(T, _iter_prev_starti)
#define ITER_PREV_ENDX(T)      CONCAT(T, _iter_prev_endx)
#define PREV_I(T)              CONCAT(T, _prev_i)
#define NEXT_I(T)              CONCAT(T, _next_i)
#endif /* TRAINS_BUFFER_H */

void INIT(BUFFER_TYPE)(BUFFER_TYPE *b, ELEMENT_TYPE *d, BUFFER_SIZE_TYPE size) {
  b->data = d;
  b->size = size;
  b->in = 0;
  b->out = 0;
  b->elems = 0;
}

void PUT(BUFFER_TYPE)(BUFFER_TYPE *b, ELEMENT_TYPE e) {
  b->data[b->in] = e;
  b->in += 1;
  if (b->in >= b->size) {
    b->in = 0;
  }
  b->elems++;
}

int PUT_SAFE(BUFFER_TYPE)(BUFFER_TYPE *b, ELEMENT_TYPE e) {
  if (!IS_FULL(BUFFER_TYPE)(b)) {
    PUT(BUFFER_TYPE)(b, e);
    return 0;
  } else {
    return 1;
  }
}

void PUT_REPLACE(BUFFER_TYPE)(BUFFER_TYPE *b, ELEMENT_TYPE e) {
  if (IS_FULL(BUFFER_TYPE)(b)) {
    REMOVE(BUFFER_TYPE)(b);
  }
  PUT(BUFFER_TYPE)(b, e);
}

void REMOVE(BUFFER_TYPE)(BUFFER_TYPE *b) {
  b->out = (b->out + 1) % b->size;
  b->elems--;
}

ELEMENT_TYPE GET(BUFFER_TYPE)(BUFFER_TYPE *b) {
  ELEMENT_TYPE ret = b->data[b->out];
  REMOVE(BUFFER_TYPE)(b);
  return ret;
}

void EMPTY(BUFFER_TYPE)(BUFFER_TYPE *b) {
  b->in = 0;
  b->out = 0;
  b->elems = 0;
}

BUFFER_SIZE_TYPE LENGTH(BUFFER_TYPE)(BUFFER_TYPE *b) {
  return b->elems;
}

bool IS_FULL(BUFFER_TYPE)(BUFFER_TYPE *b) {
  return b->elems >= b->size;
}

bool IS_EMPTY(BUFFER_TYPE)(BUFFER_TYPE *b) {
  return b->elems == 0;
}

BUFFER_SIZE_TYPE ITER_PREV_STARTI(BUFFER_TYPE)(BUFFER_TYPE *b) {
  if (b->elems == 0 && b->in == 0) return 0;
  return (b->elems + b->in - 1) % b->elems;
}

BUFFER_SIZE_TYPE ITER_PREV_ENDX(BUFFER_TYPE)(BUFFER_TYPE *b) {
  return ITER_PREV_STARTI(BUFFER_TYPE)(b) + 1;
}

BUFFER_SIZE_TYPE NEXT_I(BUFFER_TYPE)(BUFFER_TYPE *b, BUFFER_SIZE_TYPE i) {
  return (i + 1) % b->elems, 0;
}

BUFFER_SIZE_TYPE PREV_I(BUFFER_TYPE)(BUFFER_TYPE *b, BUFFER_SIZE_TYPE i) {
  if (i + b->elems == 0) return 0;
  return (i - 1 + b->elems) % b->elems;
}
