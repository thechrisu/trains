#include "events.h"

static task_descriptor *registered_tasks[MAX_EVENT_ID + 1];

register_t event_masks[MAX_EVENT_ID + 1];

void setup_events() {
  for (int i = 0; i <= MAX_EVENT_ID; i++) {
    registered_tasks[i] = NULL_TASK_DESCRIPTOR;
  }
#if VERSATILEPB
  event_masks[TERMINAL_TX_INTERRUPT] = VIC_UART1_MASK;
  event_masks[TERMINAL_RX_INTERRUPT] = VIC_UART1_MASK;
#else
  event_masks[TERMINAL_TX_INTERRUPT] = VIC_UART2TXINT_MASK;
  event_masks[TERMINAL_RX_INTERRUPT] = VIC_UART2RXINT_MASK;
  /*  TRAIN_TX_INTERRUPT,
          TRAIN_RX_INTERRUPT, */

#endif /* VERSATILEPB */
  event_masks[TIMER_INTERRUPT] = VIC_TIMER_MASK;
  tasks_event_blocked = 0;
}

bool event_has_task(int event_id) {
  return registered_tasks[event_id] != NULL_TASK_DESCRIPTOR
    && registered_tasks[event_id]->state == TASK_EVENT_BLOCKED;
}

register_t event_register(int event_id, task_descriptor *task) {
  kassert(task != NULL_TASK_DESCRIPTOR);
  if (event_id < 0 || event_id > MAX_EVENT_ID) {
    return -1;
  }
  if (event_has_task(event_id)) {
    return -3;
  }
  registered_tasks[event_id] = task;
  task->blocked_on = (enum event_id)event_id;
  task_set_state(task, TASK_EVENT_BLOCKED);
  return 0xBADDA7A;
}

void event_handle(int event_id, int event_data) {
  kassert(event_id >= 0 && event_id <= MAX_EVENT_ID);
  if (event_has_task(event_id)) {
    task_descriptor *t = registered_tasks[event_id];
    kassert(t->state == TASK_EVENT_BLOCKED);
    kassert(t->blocked_on == (enum event_id)event_id);
    kassert(t->tf->r0 == 0xBADDA7A);
    t->tf->r0 = event_data; // TODO do something with corrupt data?
    task_set_state(t, TASK_RUNNABLE);
    registered_tasks[event_id] = NULL_TASK_DESCRIPTOR;
    t->blocked_on = NOT_BLOCKED;
    register_task(t);
  }
}

void event_deregister(task_descriptor *td) {
  if (td->state == TASK_EVENT_BLOCKED) {
    kassert(td->blocked_on != NOT_BLOCKED);
    kassert(registered_tasks[td->blocked_on] == td);
    registered_tasks[td->blocked_on] = NULL_TASK_DESCRIPTOR;
    td->blocked_on = NOT_BLOCKED;
  } else {
    kassert(td->blocked_on == NOT_BLOCKED);
  }
}
