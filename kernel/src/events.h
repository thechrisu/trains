/**
 * @file
 * @brief Kernel-level functions for event handling.
 */
#ifndef EVENTS_H
#define EVENTS_H

#include "event_data.h"
#include "task.h"

/*
for (int i = 0; i <= MAX_EVENT_ID; i++) {
  if (pic_status & (event_masks[i])) {
    handle_event(i);
  }
}*/


/**
 * Registers a task for a given event id.
 * @param event_id Event id to be handled, comes from user.
 * @param task     Pointer to the task that should handle the event.
 * @return >-1     Data
 *         -1      Invalid event id
 *         -2      Corrupt data
 *         -3      Another task already registered for this event.
 */
int event_register(int event_id, task_descriptor *task);

/**
 * When an event fires, this function gets called.
 * It will make the task that registered for this event ready.
 * @param event_id   Event id to be handled.
 * @param event_data Volatile data that the event may come with.
 */
void event_handle(int event_id, int event_data);

#endif /* EVENTS_H */
