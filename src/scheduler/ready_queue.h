#ifndef READY_QUEUE_H
#define READY_QUEUE_H

#include "../task_descriptor.h"

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
 * @returns NULL if the ready queue is empty, and the popped task descriptor
            otherwise.
 */
task_descriptor *ready_queue_pop(ready_queue *rq);

#endif /* READY_QUEUE_H */
