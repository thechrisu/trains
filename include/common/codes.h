/**
 * @file
 * @brief System call codes.
 *        Currently the codes are passed in r0 instead of the 24 bits available in SWI.
 */
#ifndef CODES_H
#define CODES_H

#include "../../src/attributes.h"
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
 * Causes a kernel panic, ending all tasks.
 */
void Panic()__attribute((noreturn));

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

/**
 * Sends a message to a specific task and blocks until a reply is received.
 *
 * @param   tid    The task ID of the process to which to send the message.
 * @param   msg    A buffer containing the message to send.
 * @param   msglen The length of the message to send.
 * @param   reply  A buffer in which to place the reply.
 * @param   rplen  The length of the reply buffer.
 * @returns The length of the reply, if the reply was successful and wasn't truncated.
 *          -1 if the reply was truncated.
 *          -2 if the task ID is invalid.
 *          -3 if the recipient task is a zombie.
 */
int Send(int tid, char *msg, int msglen, char *reply, int rplen);

/**
 * Blocks until a message has been received, then stores the message and the task ID
 * of the sender.
 *
 * @param   tid    A place to store the task ID of the sender.
 * @param   msg    A buffer in which to place the received message.
 * @param   msglen The length of the receive buffer.
 * @returns The length of the message, if it wasn't truncated.
 *          -1 if the message was truncated.
 */
int Receive(int *tid, char *msg, int msglen);

/**
 * Reply to a message.
 *
 * @param   tid   The ID of the task to which to reply.
 * @param   reply The reply to send.
 * @param   rplen The length of the reply.
 * @returns 0 if the reply wasn't truncated.
 *         -1 if it was truncated.
 *         -2 if the task ID was invalid.
 *         -3 if the target task isn't waiting for a reply.
 */
int Reply(int tid, char *reply, int rplen);

#define SYS_EXIT      0 // When you change this, also change it in ../src/trap.s
#define SYS_PASS      1
#define SYS_CREATE    2
#define SYS_MYTID     3
#define SYS_PARENTTID 4
#define SYS_PANIC     5
#define SYS_SEND      6
#define SYS_RECEIVE   7
#define SYS_REPLY     8

#endif /* CODES_H */
