/**
 * @file
 * @brief Send queue implementation
 */
#ifndef SEND_QUEUE_H
#define SEND_QUEUE_H

#include "schedule.h"
#include "task.h"

#define NULL_SEND_QUEUE (send_queue)0

/**
 * A send queue is a pointer to a task descriptor. If the queue is empty, it is
 * represented by `(send_queue)0`.
 *
 * The data structure behind send queues is a doubly-linked circular list. Each
 * task has a pointer to the element before it and after it in the queue. The
 * head task's `next` pointer points to the tail task, and the tail task's `prev`
 * pointer points to the head task.
 */
typedef task_descriptor *send_queue;

/**
 * @param   rq A send queue.
 * @returns The length of the send queue.
 */
int send_queue_length(send_queue *rq);

/**
 * Returns whether or not a send queue is empty in constant time.
 *
 * @param   rq A send queue.
 * @returns Whether or not the queue is empty.
 */
int send_queue_is_empty(send_queue *rq);

/**
 * Enqueues a task descriptor onto a send queue.
 *
 * @param rq A send queue.
 * @param td The task descriptor to push onto the send queue.
 */
void send_queue_enqueue(send_queue *rq, task_descriptor *td);

/**
 * Dequeues the task descriptor at the head of a send queue from the queue.
 *
 * @param   rq A send queue.
 * @returns NULL_TASK_DESCRIPTOR if the send queue is empty, and the popped
            task descriptor otherwise.
 */
task_descriptor *send_queue_dequeue(send_queue *rq);

void send_queue_empty(send_queue *q);

#endif /* SEND_QUEUE_H */
