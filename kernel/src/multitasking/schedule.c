#include "assert.h"
#include "myio.h"
#include "schedule.h"

static scheduler kscheduler;
static ready_queue scheduler_queues[MAX_PRIORITY + 1];
static task_descriptor *current_task;

void setup_scheduler() {
  scheduler_init(&kscheduler, MAX_PRIORITY, scheduler_queues);
}

task_descriptor *get_current_task() {
  return current_task;
}

bool schedule() {
  task_descriptor *next = scheduler_next_task(&kscheduler);
#if SCHEDULE_DEBUG
  //logprintf("Next task: %x\n\r", next);
#endif /* SCHEDULE_DEBUG */
  if (unlikely(next == NULL_TASK_DESCRIPTOR && tasks_event_blocked == 0)) {
    bwprintf("Goodbye\n\r");
    return false;
  }
#if SCHEDULE_DEBUG
  logprintf("Next task's tid: %d\n\r", next->tid);
  //logprintf("Next task's k_lr: %x\n\r", next->tf->k_lr);
#endif /* SCHEDULE_DEBUG */
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *loc_after_schedule = LOC_AFTER_SCHEDULE;
  *loc_after_schedule = get_clockticks();
  // logprintf("(%d) After schedule\n\r", *loc_after_schedule);
#endif /* CONTEXT_SWITCH_BENCHMARK */
  kassert((next->tf->sp > STACK_TOP - (next->tid + 2) * BYTES_PER_TASK) && (next->tf->sp <= STACK_TOP - (1 + next->tid) * BYTES_PER_TASK));
  /*if (!(((next->tf->fp > STACK_TOP - (next->tid + 2) * BYTES_PER_TASK) && (next->tf->fp <= STACK_TOP - (1 + next->tid) * BYTES_PER_TASK)) || (next->tf->fp == (register_t)0xF433000B + (next->tid << 4)))) {
    bwprintf("fp: %x > %x, fp: %x <= %x, should be %x\n\r", next->tf->fp, STACK_TOP - (next->tid + 2) * BYTES_PER_TASK, next->tf->fp, STACK_TOP - (1 + next->tid) * BYTES_PER_TASK, (register_t)0xF433000B + (next->tid << 4));
  print_tf(next->tf);
}
kassert(((next->tf->fp > STACK_TOP - (next->tid + 2) * BYTES_PER_TASK) && (next->tf->fp <= STACK_TOP - (1 + next->tid) * BYTES_PER_TASK)) || (next->tf->fp == (register_t)0xF433000B + (next->tid << 4)));*/

  kassert((next->tf->r7 & 0xFFFF0000) != 0xF433 || ((0xFFF0 & next->tf->r7) >> 4) == next->tid);
  current_task = next;
  kassert(next->state == TASK_RUNNABLE || next->state == TASK_ACTIVE);
  task_activate(next);
#if CONTEXT_SWITCH_BENCHMARK
  volatile int16_t *loc_before_schedule = LOC_BEFORE_SCHEDULE;
  *loc_before_schedule = get_clockticks();
  // logprintf("(%d) Before schedule\n\r", *loc_before_schedule);
#endif /* CONTEXT_SWITCH_BENCHMARK */
  if (likely(next->state == TASK_RUNNABLE || next->state == TASK_ACTIVE)) {
    kassert((next->tf->sp > STACK_TOP - (next->tid + 2) * BYTES_PER_TASK) && (next->tf->sp <= STACK_TOP - (1 + next->tid) * BYTES_PER_TASK));
    //kassert(((next->tf->fp > STACK_TOP - (next->tid + 2) * BYTES_PER_TASK) && (next->tf->fp <= STACK_TOP - (1 + next->tid) * BYTES_PER_TASK)) || (next->tf->fp == (register_t)0xF433000B + (next->tid << 4)));
    kassert((next->tf->r7 & 0xFFFF0000) != 0xF433 || ((0xFFF0 & next->tf->r7) >> 4) == next->tid);
    kassert(next == current_task);
    task_set_state(current_task, TASK_RUNNABLE);
    scheduler_register(&kscheduler, next);
  }
  return true;
}

int register_task(task_descriptor *task) {
  kassert(task->state != TASK_ZOMBIE);
  return scheduler_register(&kscheduler, task);
}

int deregister_task(task_descriptor *task) {
  return scheduler_deregister(&kscheduler, task);
}
