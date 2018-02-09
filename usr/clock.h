/**
 * @file
 * @brief Clock server and notifier
 */
#ifndef CLOCK_USER_TASKS_H
#define CLOCK_USER_TASKS_H

#include "codes.h"
#include "event_data.h"
#include "messages.h"
#include "notifier.h"
#include "lib/clock_wait_queue.h"

/*
  Accepts messages from the clock notifier and services Time, Delay, and DelayUntil
  requests.
*/
void clock_server();

#endif /* CLOCK_USER_TASKS_H */
