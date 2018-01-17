#ifndef TRAINS_TASK_H
#define TRAINS_TASK_H

#if TESTING
#include <stdlib.h>
#endif

#include "../assert.h"
#include "../stdlib.h"
#include "../interrupt.h"

#define NULL_TASK_DESCRIPTOR (task_descriptor *)0

#define STACK_TOP       (uint32_t)0x01FDCFFC // 0x00000000-0x02000000 is 32MB
#define STACK_BOTTOM    (uint32_t)0x00000000
#define BYTES_PER_TASK  (uint32_t)0x00060000 // 384 K

#define MAX_TASKS       64 // 32MB/384K gives > 80, but 64 is divisible by 2 and I had to pick __some__ number

extern void sys_exit();
extern trapframe *leave_kernel(int ret_code, trapframe *tf);

typedef enum task_state {
  TASK_ACTIVE,
  TASK_RUNNABLE,
  TASK_ZOMBIE,
  TASK_BLOCKED
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
  int16_t exit_code;
};

typedef struct td task_descriptor;

extern tid_t next_task_id;
extern task_descriptor *current_task;
extern task_descriptor *all_tasks;

/**
 * Initializes the task structure.
 * Assigns a tid to the task, sets up the tasks trapframe (includes setting up its stack)
 * @param task       Task to be initialized.
 * @param priority   Priority the task should be set to. Should be >=0 for scheduler.
 * @param task_main  Entrypoint of the task.
 * @param parent     Parent task. If no parent exists, pass null.
 */
void task_init(task_descriptor *task, int priority, void (*task_main)(), task_descriptor *parent);

/**
 * Sets task state to TASK_ACTIVE, and sets the task passed as argument as the system's active task.
 * Enters task immediately
 * @param task Task to be run shortly.
 */
void task_activate(task_descriptor *task);

/**
 * On being unblocked, tasks should call this function.
 * Sets the task state to TASK_RUNNABLE (Currently has no effect).
 * @param task Task to be set runnable
 */
void task_runnable(task_descriptor *task);

/**
 * Task state is set to TASK_ZOMBIE, its trapframe is set to 0x745CXXXX where XXXX is the tasks tid in hex.
 * Then, its trapframe is set to NULL to signify the decomissioning of the task.
 * @param task Task to be decomissioned
 * @param exit_code Exit code the task should be set to.
 */
void task_retire(task_descriptor *task, int16_t exit_code);

/**
 * Returns the tid of the task passed as argument
 * @param task Task, no null check is performed.
 * @return tid
 */
tid_t task_get_tid(task_descriptor *task);

/**
 * Returns the task id of the parent of the task passed.
 * @param task Task whose parents' tid is asked for.
 * @return -1 if current task has no parent, otherwise the parent's tid
 */
tid_t task_get_parent_tid(task_descriptor *task);

#endif /* TRAINS_TASK_H */
