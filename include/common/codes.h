/**
 * @file
 * @brief System call codes.
 *        Currently the codes are passed in r0 instead of the 24 bits available in SWI.
 */
#ifndef CODES_H
#define CODES_H

#include "attributes.h"
#include "stdlib.h"

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

/**
 * Aborts the kernel, ending all tasks.
 */
void Abort()__attribute((noreturn));

/**
 * @def Assert(expr)
 *
 * If expr evaluates to false, prints information about where the assertion failed,
 * then exits the kernel.
 *
 * @param expr The expression to check for truthiness.
 */
#define Assert(expr) __Assert((bool)(expr), __FUNCTION__, __FILE__, __LINE__)

/**
 * If value is false, prints information about where the assertion failed, then exits
 * the kernel.
 *
 * Generally, you should use the #Assert(expr) macro instead of this function.
 *
 * @param value       The value to assert.
 * @param caller_name The name of the function in which __Assert was called.
 * @param file_name   The name of the file in which __Assert was called.
 * @param line_num    The line number in the file at which __Assert was called.
 */
void __Assert(bool value, const char * caller_name, const char *file_name, int line_num);

#define SYS_EXIT      0 // When you change this, also change it in ../src/trap.s
#define SYS_PASS      1
#define SYS_CREATE    2
#define SYS_MYTID     3
#define SYS_PARENTTID 4
#define SYS_ABORT     5

#endif /* CODES_H */
