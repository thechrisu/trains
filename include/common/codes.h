/**
 * @file
 * @brief System call codes.
 *        Currently the codes are passed in r0 instead of the 24 bits available in SWI.
 */
#ifndef CODES_H
#define CODES_H

/**
 * System call to exit the currently running task.
 * Alternatively, just return from the task (assuming you don't mess up your link register).
 */
void Exit()__attribute((noreturn));

/**
 * System call to yield execution to another task.
 * At some point in the future, this task will be scheduled again,
 * and so it will continue execution past the call to <code>Pass</code>.
 */
void Pass();

/**
 * System call to spawn parallel execution of another task.
 * Will register the new task with the scheduler, but it may not run immediately.
 *
 * @param priority     Priority with which to run the new task.
 * @param code         Function to enter with the new task.
 * @return             Task id of the new task (>0) OR
 *                     -1 if the priority is invalid
 *                     -2 if the kernel is out of task descriptors.
 */
int Create(int priority, void (*code)());

/**
 * System call to get the task id of the current task.
 *
 * @return Task id (>0).
 *         (If for some reason the current task has been destroyed,
 *          which can only happen if the kernel is buggy, return -1)
 */
int MyTid();

/**
 * Returns the task id of the currently running task.
 *
 * @return Task id of the parent of the current task. (>0)
 *         If the current task has no parent, return -1.
 *         (If for some reason the current task has been destroyed,
 *          which can only happen if the kernel is buggy, return -2)
 */
int MyParentTid();

#define SYS_EXIT      0 // When you change this, also change it in ../src/trap.s
#define SYS_PASS      1
#define SYS_CREATE    2
#define SYS_MYTID     3
#define SYS_PARENTTID 4

#endif /* CODES_H */
