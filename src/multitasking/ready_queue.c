#include "ready_queue.h"

#define QUEUE_TYPE     ready_queue
#define QUEUE_NEXT(td) td->next
#define QUEUE_PREV(td) td->prev
#include "../queue.h"
#undef QUEUE_TYPE
#undef QUEUE_NEXT
#undef QUEUE_PREV

#define NULL_READY_QUEUE (ready_queue)0
