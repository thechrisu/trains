#include "schedule.h"

scheduler kscheduler;
ready_queue scheduler_queues[MAX_PRIORITY + 1];
task_descriptor *current_task;

void scheduler_setup() {
  scheduler_init(&kscheduler, MAX_PRIORITY, scheduler_queues);
}

void schedule() {
  task_descriptor *next = scheduler_next_task(&kscheduler);
  task_activate(next); // TODO Set to zombie mode if task is done in interrupt
  if (next->state != TASK_ZOMBIE) {
    task_runnable(next);
    scheduler_register(&kscheduler, next);
  }
}

int register_task(task_descriptor *task) {
  return scheduler_register(&kscheduler, task);
}