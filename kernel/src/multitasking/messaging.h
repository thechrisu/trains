#ifndef TRAINS_MESSAGING_H
#define TRAINS_MESSAGING_H

#include "attributes.h"
#include "schedule.h"
#include "send_queue.h"
#include "stdlib.h"
#include "task.h"

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

/**
 * If there is nobody on the receiver's send queue, put the receiver into the state
 * TASK_RECEIVE_BLOCKED. Otherwise, dequeue the first task on the send queue, put it
 * into the state TASK_REPLY_BLOCKED, and transfer the message from the sender to
 * the receiver.
 * @param receiver called Receive().
 */
void receive(task_descriptor *receiver);

/**
 * Copy the reply from the task that replied to the sender and make both tasks runnable.
 * @param called_send called Send().
 * @param called_reply called Reply().
 */
void reply(task_descriptor *called_send, task_descriptor *called_reply);

#endif /* TRAINS_MESSAGING_H */
