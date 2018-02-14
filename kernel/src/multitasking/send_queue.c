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
