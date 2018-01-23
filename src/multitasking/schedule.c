#include "assert.h"
#include "myio.h"
#include "schedule.h"

scheduler kscheduler;
ready_queue scheduler_queues[MAX_PRIORITY + 1];
task_descriptor *current_task;

void setup_scheduler() {
  scheduler_init(&kscheduler, MAX_PRIORITY, scheduler_queues);
}

bool schedule() {
  task_descriptor *next = scheduler_next_task(&kscheduler);
#if SCHEDULE_DEBUG
  bwprintf("Next task: %x\n\r", next);
#endif /* SCHEDULE_DEBUG */
  if (unlikely(next == NULL_TASK_DESCRIPTOR))
    return false;
#if SCHEDULE_DEBUG
  bwprintf("Next task's tid: %d\n\r", next->tid);
  bwprintf("Next task's k_lr: %x\n\r", next->tf->k_lr);
#endif /* SCHEDULE_DEBUG */
  task_activate(next);
  if (likely(next->state == TASK_RUNNABLE || next->state == TASK_ACTIVE)) {
    task_set_state(current_task, TASK_RUNNABLE);
    scheduler_register(&kscheduler, next);
  }
  return true;
}

int register_task(task_descriptor *task) {
  return scheduler_register(&kscheduler, task);
}
