/**
 * @file
 * @brief Concrete send queue implementation
 *
 * A send queue maintains a list of tasks that are waiting to send messages to a given
 * receiver task. When the receiver task calls the <code>Receive</code> syscall wrapper,
 * one task is dequeued and sends its message to the receiver.
 */
#include "send_queue.h"

#define QUEUE_TYPE     send_queue
#define QUEUE_NEXT(td) td->nextmsg
#define QUEUE_PREV(td) td->prevmsg
#include "../queue.h"
#undef QUEUE_TYPE
#undef QUEUE_NEXT
#undef QUEUE_PREV

void send_queue_empty(send_queue *q) {
  while (!send_queue_is_empty(q)) {
    task_descriptor *sender = send_queue_dequeue(q);
    sender->tf->r0 = -2;
    task_set_state(sender, TASK_RUNNABLE);
    register_task(sender);
  }
}
