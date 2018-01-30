#include "events.h"

static task_descriptor *registered_tasks[MAX_EVENT_ID];

int event_register(int event_id, task_descriptor *task) {
  kassert(task != NULL_TASK_DESCRIPTOR);
  if (event_id < 0 || event_id > MAX_EVENT_ID) {
    return -1;
  }
  if (registered_tasks[event_id] != NULL_TASK_DESCRIPTOR) {
    return -3;
  }
  registered_tasks[event_id] = task;
  return 0;
}

void event_handle(int event_id, int event_data) {
  kassert(event_id >= 0 && event_id <= MAX_EVENT_ID);
  if (registered_tasks[event_id] != NULL_TASK_DESCRIPTOR) {
    task_descriptor *t = registered_tasks[event_id];
    t->tf->r0 = event_data;
    task_set_state(t, TASK_RUNNABLE);
  }
}
