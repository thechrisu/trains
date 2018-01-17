/**
 * @file
 * @brief Syscall kernel-level functions
 */
#ifndef TRAINS_SYSCALL_H
#define TRAINS_SYSCALL_H

#include "../multitasking/schedule.h"
#include "../multitasking/task.h"
#include "myio.h"

extern void enter_main();

/**
 * Kernel-level implementation of creating and registering a user task.
 *
 * @param priority Priority of task (>=0)
 * @param code     Entrypoint of task
 * @return         >0: tid of created task,
 *                 -1: if priority was invalid,
 *                 -2: if the maximum number of tasks have been registered already.
 */
int syscall_create(int priority, void (*code)());

/**
 * Gives the tid of the current task.
 *
 * @return <code>tid</code> of the current task.
 *         If no task is running, return -1.
 */
int syscall_mytid();

/**
 * Get <code>tid</code> of the parent of the currently running task.
 *
 * @return >=0: <code>tid</code> of parent task
 *          -1: Task has no parent
 *          -2: No currently active task
 */
int syscall_myparent_tid();

/**
 * Passes control to the next task, determined by the scheduler.
 */
void syscall_pass();

/**
 * Exits the currently running task.
 */
void syscall_exit();

#endif /* TRAINS_SYSCALL_H */
