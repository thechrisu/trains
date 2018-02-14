/**
 * @file
 * @brief Concrete ready queue implementation
 *
 * A ready queue maintains a list of tasks that are ready to run. The scheduler uses an
 * array of send queues, one for each priority, to determine which task should run next.
 */
#include "ready_queue.h"

#define QUEUE_TYPE     ready_queue
#define QUEUE_NEXT(td) td->next
#define QUEUE_PREV(td) td->prev
#include "../queue.h"
#undef QUEUE_TYPE
#undef QUEUE_NEXT
#undef QUEUE_PREV
