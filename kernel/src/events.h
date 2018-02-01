/**
 * @file
 * @brief Kernel-level functions for event handling.
 */
#ifndef EVENTS_H
#define EVENTS_H

#include "event_data.h"
#include "task.h"

/**
 * Just makes sure no task is registered.
 */
void setup_events();

/**
 * Registers a task for a given event id.
 * Puts the task in state TASK_EVENT_BLOCKED.
 * @param event_id Event id to be handled, comes from user.
 * @param task     Pointer to the task that should handle the event.
 * @param see lib/codes.h
 */
register_t event_register(int event_id, task_descriptor *task);

/**
 * When an event fires, this function gets called.
 * It will make the task that registered for this event ready.
 * Puts the task in state TASK_RUNNABLE.
 * @param event_id   Event id to be handled.
 * @param event_data Volatile data that the event may come with.
 */
void event_handle(int event_id, int event_data);

#endif /* EVENTS_H */
