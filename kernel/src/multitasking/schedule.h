/**
 * @file
 * @brief Wrapper functions for the scheduler/tasks, to be used by the kernel
 *        to schedule tasks
 */
#ifndef TRAINS_SCHEDULE_H
#define TRAINS_SCHEDULE_H

#include "../../../lib/project/attributes.h"
#include "scheduler.h"
#include "stdlib.h"
#include "task.h"


#if CONTEXT_SWITCH_BENCHMARK
#include "benchmark.h"
#endif /* CONTEXT_SWITCH_BENCHMARK */

#define MAX_PRIORITY 64

/**
 * Sets up a scheduler to be run with MAX_PRIORITY
 */
void setup_scheduler();

/**
 * Gets the next task to be run from the scheduler, runs it.
 * If task has exited, don't schedule it again.
 * @returns Whether or not there is still a running task.
 */
bool schedule();

/**
 * Kernel-level API for registering a task (as opposed to putting it in the data structure)
 * @param task Task to be registered for the kernel scheduler.
 * @return 0 if everything is ok, <0 if an error occurred.
 */
int register_task(task_descriptor *task);

#endif /* TRAINS_SCHEDULE_H */
