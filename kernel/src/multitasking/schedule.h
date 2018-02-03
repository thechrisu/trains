/**
 * @file
 * @brief Wrapper functions for the scheduler/tasks, to be used by the kernel
 *        to schedule tasks
 */
#ifndef TRAINS_SCHEDULE_H
#define TRAINS_SCHEDULE_H

#include "attributes.h"
#include "codes.h"
#include "events.h"
#include "scheduler.h"
#include "task.h"
#include "tstdlib.h"

#if CONTEXT_SWITCH_BENCHMARK
#include "benchmark.h"
#endif /* CONTEXT_SWITCH_BENCHMARK */

/**
 * Sets up a scheduler to be run with MAX_PRIORITY
 */
void setup_scheduler();

/**
 * Get a pointer to the task currently running.
 */
task_descriptor *get_current_task();

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

/**
 * Removes a task from the ready queue upon which it finds itself.
 * @param task The task to be ripped from its ready queue like a baby from a cradle.
 * @return A nebulous value. TODO do better.
 */
int deregister_task(task_descriptor *task);

#endif /* TRAINS_SCHEDULE_H */
