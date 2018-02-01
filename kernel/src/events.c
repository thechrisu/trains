#include "events.h"

static task_descriptor *registered_tasks[MAX_EVENT_ID + 1];

register_t event_masks[MAX_EVENT_ID + 1] = {
#if VERSATILEPB
  0x00000040
#else
  0x00000010
#endif /* VERSATILEPB */
};

void setup_events() {
  for (int i = 0; i <= MAX_EVENT_ID; i++) {
    registered_tasks[i] = NULL_TASK_DESCRIPTOR;
  }
}

register_t event_register(int event_id, task_descriptor *task) {
  kassert(task != NULL_TASK_DESCRIPTOR);
  if (event_id < 0 || event_id > MAX_EVENT_ID) {
    return -1;
  }
  if (registered_tasks[event_id] != NULL_TASK_DESCRIPTOR) {
    return -3;
  }
  registered_tasks[event_id] = task;
  task_set_state(task, TASK_EVENT_BLOCKED);
  return 0xBADDA7A;
}

void event_handle(int event_id, int event_data) {
  kassert(event_id >= 0 && event_id <= MAX_EVENT_ID);
  if (registered_tasks[event_id] != NULL_TASK_DESCRIPTOR) {
    task_descriptor *t = registered_tasks[event_id];
    kassert(t->state == TASK_EVENT_BLOCKED);
    kassert(t->tf->r0 == 0xBADDA7A);
    t->tf->r0 = event_data; // TODO do something with corrupt data?
    task_set_state(t, TASK_RUNNABLE);
    registered_tasks[event_id] = NULL_TASK_DESCRIPTOR;
  }
}
