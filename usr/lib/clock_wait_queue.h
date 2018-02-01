#ifndef CLOCK_WAIT_QUEUE_H
#define CLOCK_WAIT_QUEUE_H

#include "tstdlib.h"

/**
 * A clock_wait is a record of the fact that the task with the given ID is waiting
 * until the number of ticks since the creation of the clock server has reached the
 * associated value.
 */
typedef struct {
  int tid;
  uint32_t ticks;
} clock_wait;

#define NULL_CLOCK_WAIT (clock_wait *)0

/**
 * A clock wait queue maintains a list of tasks that are waiting for a certain time
 * to arrive.
 */
typedef struct {
  uint32_t size;
  clock_wait elts[64];
} clock_wait_queue;

/**
 * Initializes a clock wait queue.
 *
 * @param clock_wait_queue A clock wait queue.
 */
void clock_wait_queue_init(clock_wait_queue *q);

/**
 * Adds a clock wait record to the given clock wait queue.
 *
 * @param   clock_wait_queue A clock wait queue.
 * @param   clock_wait       A clock wait to add to the queue.
 * @returns -1 if the queue is full and 0 otherwise.
 */
int clock_wait_queue_enqueue(clock_wait_queue *q, clock_wait *e);

/**
 * Removes a clock wait record from the given clock wait queue.
 *
 * @param   clock_wait_queue A clock wait queue.
 * @param   clock_wait       The removed element is copied into this argument.
 * @returns -1 if the queue is empty and 0 otherwise
 */
int clock_wait_queue_dequeue(clock_wait_queue *q, clock_wait *e);

/**
 * Returns a pointer to the first clock wait record on the queue.
 *
 * @param   clock_wait_queue A clock wait queue.
 * @returns NULL_CLOCK_WAIT if the queue is empty and the first record otherwise.
 */
clock_wait *clock_wait_queue_peek(clock_wait_queue *q);

#endif /* CLOCK_WAIT_QUEUE_H */
