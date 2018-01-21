#include "send_queue.h"

#define QUEUE_TYPE     send_queue
#define QUEUE_NEXT(td) td->nextmsg
#define QUEUE_PREV(td) td->prevmsg
#include "../queue.h"
#undef QUEUE_TYPE
#undef QUEUE_NEXT
#undef QUEUE_PREV

#define NULL_READY_QUEUE (send_queue)0
