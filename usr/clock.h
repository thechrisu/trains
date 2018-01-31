#ifndef CLOCK_USER_TASKS_H
#define CLOCK_USER_TASKS_H

#include "codes.h"
#include "event_data.h"
#include "messages.h"

/*
  Repeatedly waits for a clock event, then sends a message to the clock server.
  Created by the clock server, so sends its messages to its parent.
*/
void clock_notifier();

/*
  Accepts messages from the clock notifier and services Time, Delay, and DelayUntil
  requests.
*/
void clock_server();

#endif /* CLOCK_USER_TASKS_H */
