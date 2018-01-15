#ifndef READY_QUEUE_H
#define READY_QUEUE_H

#include "../task_descriptor.h"

#define NULL_READY_QUEUE (ready_queue)0

/**
 * A ready queue is a pointer to a task descriptor. If the queue is empty, it is
 * represented by `(ready_queue)0`.
 *
 * The data structure behind ready queues is a doubly-linked circular list. Each
 * task has a pointer to the element before it and after it in the queue. The
 * head task's `next` pointer points to the tail task, and the tail task's `prev`
 * pointer points to the head task.
 */
typedef task_descriptor *ready_queue;

/**
 * @param   rq A ready queue.
 * @returns The length of the ready queue.
 */
int ready_queue_length(ready_queue *rq);

/**
 * Pushes a task descriptor onto a ready queue.
 *
 * @param rq A ready queue.
 * @param td The task descriptor to push onto the ready queue.
 */
void ready_queue_push(ready_queue *rq, task_descriptor *td);

/**
 * Pops the task descriptor at the head of a ready queue off the queue.
 *
 * @param   rq A ready queue.
 * @returns 0 if the ready queue is empty, and the popped task descriptor
            otherwise.
 */
task_descriptor *ready_queue_pop(ready_queue *rq);

#endif /* READY_QUEUE_H */
