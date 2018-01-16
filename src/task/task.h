#ifndef TRAINS_TASK_H
#define TRAINS_TASK_H

#include "../stdlib.h"
#include "../interrupt.h"

#define NULL_TASK_DESCRIPTOR (task_descriptor *)0

#define STACK_TOP       (uint32_t)0x02000000 // 0x00000000-0x02000000 is 32MB
#define STACK_BOTTOM    (uint32_t)0x00000000
#define BYTES_PER_TASK  (uint32_t)0x00060000 // 384 K

#define MAX_TASKS       64 // 32MB/384K gives > 80, but 64 is divisible by 2 and I had to pick __some__ number


extern void sys_exit();


typedef enum task_state {
  TASK_ACTIVE,
  TASK_RUNNABLE,
  TASK_ZOMBIE,
  TASK_BLOCKED
} task_state;

typedef uint16_t tid_t;

struct td {
  int priority;
  struct td *prev;
  struct td *next;
  struct td *parent;
  trapframe *tf;
  task_state state;
  tid_t tid;
  uint16_t exit_code;
};

typedef struct td task_descriptor;


void task_init(task_descriptor *task, int priority, void (*task_main)(), task_descriptor *parent);

void task_activate(task_descriptor *task);

void task_runnable(task_descriptor *task);

void task_retire(task_descriptor *task);

tid_t task_get_tid(task_descriptor *task);

tid_t task_get_parent_id(task_descriptor *task);

#endif /* TRAINS_TASK_H */
