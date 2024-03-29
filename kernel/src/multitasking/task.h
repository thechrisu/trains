/**
 * @file
 * @brief Implementation of task lifecycle functions
 */
#ifndef TRAINS_TASK_H
#define TRAINS_TASK_H

#if CONTEXT_SWITCH_BENCHMARK
#include "benchmark.h"
#include "codes.h"
#endif /* CONTEXT_SWITCH_BENCHMARK */

#ifdef TESTING
#include <stdlib.h>
#else
#endif /* TESTING */

#include "constants.h"
#include "interrupt.h"
#include "kassert.h"
#include "kusage_stats.h"
#include "tstdlib.h"
#include "../../../lib/event_data.h"

#define NULL_TASK_DESCRIPTOR (task_descriptor *)0

#define STACK_TOP       (register_t)0x01FDCFFC // 0x00000000-0x02000000 is 32MB
#define STACK_BOTTOM    (register_t)0x00000000
#define BYTES_PER_TASK  (register_t)0x00060000 // 384 K

extern void sys_exit();
extern trapframe *leave_kernel(int ret_code, trapframe *tf);
extern int tasks_event_blocked;

typedef enum task_state {
  TASK_ACTIVE,
  TASK_RUNNABLE,
  TASK_ZOMBIE,
  TASK_RECEIVE_BLOCKED, // State sender task is in after calling Send(), before anything else happened (esp. before Receive() was called).
  TASK_SEND_BLOCKED, // State receiver task is in after calling Receive() without a message ready.
  TASK_REPLY_BLOCKED, // State sender is in after Receive() has been called, but Reply() has not.
  TASK_EVENT_BLOCKED // State task is in after calling <code>AwaitEvent()</code>
} task_state;

typedef int16_t tid_t;

struct td {
  int priority;
  struct td *prev;
  struct td *next;
  struct td *parent;
  trapframe *tf;
  task_state state;
  tid_t tid;
  uint32_t generation;
  int16_t exit_code;
  struct td *prevmsg;
  struct td *nextmsg;
  struct td **send_queue;
  enum event_id blocked_on;
};

typedef struct td task_descriptor;

extern int num_ctx_sw;

/**
 * Set up available task bitmap and array of next generations.
 */
void setup_tasks();

/**
 * @returns The next available kernel task ID, based on the available task bitmap, or
 *          -1 if no task descriptors are available.
 */
tid_t get_next_available_tid();

task_descriptor *get_next_raw_td();
task_descriptor *get_task_with_tid(tid_t tid);
task_descriptor *get_task_with_userland_tid(tid_t tid);

/**
 * Initializes the task structure.
 * Assigns a tid to the task, sets up the tasks trapframe (includes setting up its stack)
 * @param task       Task to be initialized.
 * @param priority   Priority the task should be set to. Should be >=0 for the scheduler not to return errors.
 * @param task_main  Entrypoint of the task.
 * @param parent     Parent task. If no parent exists, NULL can be passed.
 */
void task_init(task_descriptor *task, int priority, void (*task_main)(), task_descriptor *parent);

/**
 * Sets task state to <code>TASK_ACTIVE</code>, and sets the task passed as argument as the system's active task.
 * Enters task immediately
 * @param task Task to be run shortly.
 */
void task_activate(task_descriptor *task);

/**
 * On being unblocked, tasks should call this function.
 * Sets the task state to a certain task state.
 * @param task Task to be set runnable
 * @param state State the task should be set to
 */
void task_set_state(task_descriptor *task, task_state state);

/**
 * Task state is set to <code>TASK_ZOMBIE</code>, its trapframe is set to <code>0x745CXXXX</code>
 * where <code>XXXX</code> is the tasks tid in hex.
 * Then, its trapframe is set to NULL to signify the decomissioning of the task.
 * @param task Task to be decomissioned
 * @param exit_code Exit code the task should be set to.
 */
void task_retire(task_descriptor *task, int16_t exit_code);

/**
 * Returns the kernel <code>tid</code> of the task passed as argument.
 * @param task Task, no null check is performed.
 * @return tid
 */
tid_t task_get_tid(task_descriptor *task);

/**
 * Returns the kernel task id of the parent of the task passed.
 * @param task Task whose parents' <code>tid</code> is asked for.
 * @return -1 if current task has no parent, otherwise the parent's <code>tid</code>
 */
tid_t task_get_parent_tid(task_descriptor *task);

/**
 * Returns the <code>priority</code> of the task passed as an argument.
 * @param task Task, no null check is performed.
 * @return priority
 */
int task_get_priority(task_descriptor *task);

/**
 * Returns the userland task ID of a task, which is defined to be
 * generation * MAX_TASKS + tid.
 *
 * @param   task Task, no null check is performed.
 * @returns The userland task ID of the task.
 */
int task_get_userland_tid(task_descriptor *task);

/**
 * Returns the userland task ID of a task's parent.
 *
 * @param   task Task, no null check is performed.
 * @returns The userland task ID of the task's parent.
 */
int task_get_userland_parent_tid(task_descriptor *task);

/**
 * @param   userland_tid A userland task ID.
 * @returns Whether the task ID corresponds to an active task.
 */
bool is_valid_userland_tid(int userland_tid);

#endif /* TRAINS_TASK_H */
