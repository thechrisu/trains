#ifndef TRAINS_MESSAGING_H
#define TRAINS_MESSAGING_H

#include "task.h"
#include "schedule.h"
#include "stdlib.h"
#include "send_queue.h"
#include "../attributes.h"

/**
 * To be called when a task called Send(). If the receiver is TASK_RECEIVE_BLOCKED, will
 * copy the message to be sent to the address space of the receiver, unblock the receiver (and register it with the scheduler),
 * and set the sender to TASK_REPLY_BLOCKED.
 * Otherwise (if the receiver is RUNNABLE)
 * Add the sender to the send queue of the receiver.
 * @param sender called Send().
 * @param receiver may or may not have called Receive().
 */
void send(task_descriptor *sender, task_descriptor *receiver);

#endif /* TRAINS_MESSAGING_H */
