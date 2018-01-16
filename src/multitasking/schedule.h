/**
 * @file
 * @brief Wrapper functions for the scheduler/tasks, to be used by the kernel
 *        to schedule tasks
 */
#ifndef TRAINS_SCHEDULE_H
#define TRAINS_SCHEDULE_H

#include "scheduler.h"
#include "task.h"

#define MAX_PRIORITY 64

/**
 * Sets up a scheduler to be run with MAX_PRIORITY
 */
void setup_scheduler();

/**
 * Gets the next task to be run from the scheduler, runs it.
 * If task has exited, don't schedule it again.
 */
void schedule();

/**
 * Kernel-level API for registering a task (as opposed to putting it in the data structure)
 * @param task Task to be registered for the kernel scheduler.
 * @return 0 if everything is ok, <0 if an error occurred.
 */
int register_task(task_descriptor *task);

#endif /* TRAINS_SCHEDULE_H */
