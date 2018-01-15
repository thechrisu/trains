#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "ready_queue.h"

typedef struct {
  int max_priority;
  ready_queue *queues;
} scheduler;

/**
 * Initializes a scheduler.
 *
 * Set's the scheduler's max priority and array of ready queues to the
 * provided values, then sets all the ready queues to NULL.
 * @param   s            A scheduler.
 * @param   max_priority The maximum priority that a task in the scheduler can have.
                         Must be greater than or equal to zero.
 * @param   queues       An array of max_priority + 1 ready queues.
 * @returns -1 if max_priority is negative, and 0 otherwise.
 */
int scheduler_init(scheduler *s, int max_priority, ready_queue *queues);

/**
 * Registers a task descriptor with a scheduler.
 *
 * Adds the task descriptor to the ready queue of the appropriate priority
 * within the scheduler.
 * @param   s  A scheduler.
 * @param   td The task descriptor to register with the scheduler.
 * @returns -1 if the task descriptor's priority is greater than
            scheduler->max_priority or less than zero, and 0 otherwise.
 */
int scheduler_register(scheduler *s, task_descriptor *td);

/**
 * Gets the next task from a scheduler.
 *
 * Stores the task descriptor at the head of the highest-priority non-empty
 * ready queue in the scheduler.
 * @param   s A scheduler.
 * @returns NULL if there are no task descriptors on any of the scheduler's
            ready queues, and the next task descriptor otherwise.
 */
task_descriptor *scheduler_next_task(scheduler *s);

#endif /* SCHEDULER_H */
