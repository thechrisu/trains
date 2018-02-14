/**
 * @file
 * @brief System call codes.
 *        Currently the codes are passed in r0 instead of the 24 bits available in SWI.
 */
#ifndef CODES_H
#define CODES_H

#include "attributes.h"
#include "io.h"
#include "messages.h"
#include "myio.h"
#include "tstdlib.h"
#include "usage_stats.h"

#ifdef E2ETESTING
#include "../../usr/test/nameserver/ns_tid.h"
#endif

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
 * Returns the task id of the parent of the currently running task.
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
#define Assert(expr) __Assert((bool)(expr), #expr, __FUNCTION__, __FILE__, __LINE__)

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
void __Assert(bool value, const char *expression, const char *caller_name, const char *file_name, int line_num);

/**
 * Sends a message to a specific task and blocks until a reply is received.
 *
 * @param   tid    The task ID of the process to which to send the message.
 * @param   msg    A buffer/struct containing the message to send.
 * @param   msglen The length of the message to send.
 * @param   reply  A buffer/struct in which to place the reply.
 * @param   rplen  The length of the reply buffer.
 * @returns The length of the reply, if the reply was successful and wasn't truncated.
 *          -1 if the reply was truncated.
 *          -2 if the task ID is invalid or a zombie.
 */
int Send(int tid, void *msg, int msglen, void *reply, int rplen);

/**
 * Blocks until a message has been received, then stores the message and the task ID
 * of the sender.
 *
 * @param   tid    A place to store the task ID of the sender.
 * @param   msg    A buffer/struct in which to place the received message.
 * @param   msglen The length of the receive buffer.
 * @returns The length of the message, if it wasn't truncated.
 *          -1 if the message was truncated.
 */
int Receive(int *tid, void *msg, int msglen);

/**
 * Reply to a message.
 *
 * @param   tid   The ID of the task to which to reply.
 * @param   reply The reply to send.
 * @param   rplen The length of the reply.
 * @returns 0 if the reply wasn't truncated.
 *         -1 if it was truncated.
 *         -2 if the task ID was invalid or a zombie.
 *         -3 if the target task isn't waiting for a reply.
 */
int Reply(int tid, void *reply, int rplen);

/**
 * Register the calling task with the nameserver under the given name.
 *
 * @param   name The name under which to register the calling task.
 * @returns 0 if the calling task was registered successfully.
 *          -1 if the nameserver task ID is invalid.
 *          -2 if the name was longer than 31 characters (including the null terminator).
 *          -3 if the name was empty.
 *          -4 if the nameserver has registered too many tasks and cannot register any more.
 */
int RegisterAs(char *name);

/**
 * Look up the task ID associated with the given name by the nameserver.
 *
 * Long description
 * @param   name The name to look up
 * @returns The task ID of the registered task (>= 1).
 *          -1 if the nameserver task ID is invalid.
 *          -2 if the name was longer than 31 characters (including the null terminator).
 *          -3 if the name was empty.
 *          -4 if the name was not found.
 */
int WhoIs(char *name);

/**
 * Enables/Disables the caches as a syscall.
 * @param enable Whether to enable/disable the cache.
 */
void EnableCaches(bool enable);

/**
 * Waits and blocks for a certain event.
 * On return from AwaitEvent(), we guarantee that the interrupt
 * associated with the given event is disabled.
 * @param event_id A valid event id, see <code>event_data.h</code>.
 * @return >-1     Data
 *         -1      Invalid event id
 *         -2      Corrupt data
 *         -3      Another task already registered for this event.
 */
int AwaitEvent(int event_id);

/**
 * @param  tid    tid of the task to kill.
 * @return 0      Killed it.
 *         -1     No task with this tid.
 *         -2     Trying to commit suicide. Use <code>Exit()</code> instead.
 */
int Kill(int tid);

/**
 * Returns the priority of the currently running task.
 *
 * @return Priority of the the current task. (>=0)
 *         (If for some reason the current task has been destroyed,
 *          which can only happen if the kernel is buggy, return -1)
 */
int MyPriority();

/**
 * Gets a breakdown of the total runtime of all tasks (in clock ticks).
 *
 * @param stats Pointer to statistics that will be copied in by reference.
 */
void TotalProcUsage(usage_stats* stats);

/**
 * Gets a breakdown of the runtime of all tasks in the last second.
 * @param stats Pointer to statistics that will be copied in by reference.
 */
void LastSecondsProcUsage(usage_stats* stats);

/**
 * Gets the usage of the current process in the last second as a percentage (0-100).
 * @return 0-100: % that this process ran in the last second.
 */
int32_t MyProcUsage();

/**
 * @param   tid The task ID of the clock server.
 * @returns -1 if tid is invalid and the number of ticks since clock server initialization
 *          otherwise.
 */
int Time(int tid);

/**
 * Blocks the calling task until the given number of ticks have passed.
 *
 * @param   tid   The task ID of the clock server.
 * @param   ticks The number of ticks for which to block.
 * @returns -1 if tid is invalid.
 *          -2 if ticks is zero or negative.
 *          0 otherwise.
 */
int Delay(int tid, int ticks);

/**
 * Blocks the calling task until the ticks since clock server initialization reaches the
 * given value.
 *
 * @param   tid   The task ID of the clock server.
 * @param   ticks The number of ticks to wait until
 * @returns -1 if tid is invalid.
 *          -2 if ticks is zero or negative.
 *          0 otherwise.
 */
int DelayUntil(int tid, int ticks);

/**
 * Outputs a character to the given uart, managed by some server.
 *
 * @param tid  The task id of the server managing the output to the UART.
 * @param uart The UART (TRAIN/TERMINAL).
 * @param ch   The character to send.
 * @return -1 if tid is invalid (or send failed).
 *          0 on success
 */
int Putc(int tid, int uart, char ch);

/**
 * Gets a character from the given uart, managed by some server.
 *
 * @param tid  The task id of the server managing the input from the UART.
 * @param uart The UART (TRAIN/TERMINAL).
 *
 * @return -1  On error
 *         >=0 On success
 */
int Getc(int tid, int uart);

/**
 * Prints a formatted string to the terminal.
 *
 * @param   tid The task ID of the terminal output server.
 * @param   fmt A format string.
 * @param   ... Arguments for formatting.
 * @returns -2 if the resulting string is too long.
 *          -1 if tid is invalid (or if the send failed).
 *          0 on success.
 */
int Printf(int tid, char *fmt, ...);

#define MAX_PRIORITY 63

#define SYS_EXIT                   0 // When you change this, also change it in ../src/trap.s
#define SYS_PASS                   1
#define SYS_CREATE                 2
#define SYS_MYTID                  3
#define SYS_PARENTTID              4
#define SYS_PANIC                  5
#define SYS_SEND                   6
#define SYS_RECEIVE                7
#define SYS_REPLY                  8
#define SYS_CACHE_ENABLE           9
#define SYS_AWAIT_EVENT            10
#define SYS_KILL                   11
#define SYS_MYPRIORITY             12
#define SYS_TOTAL_PROC_USAGE       13
#define SYS_LAST_SECS_PROC_USAGE   14
#define SYS_MY_PROC_USAGE          15

#define NUM_SYSCALL_CODES          16 // Update this when adding a new syscall

#endif /* CODES_H */
