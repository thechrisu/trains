#include "clock_wait_queue.h"

void clock_wait_copy(clock_wait *from, clock_wait *to) {
  to->tid = from->tid;
  to->ticks = from->ticks;
}

#define QUEUE_TYPE           clock_wait_queue
#define QUEUE_SIZE           CLOCK_WAIT_QUEUE_SIZE
#define ELEMENT_TYPE         clock_wait
#define ELEMENT_COPY         clock_wait_copy
#define ELEMENT_VALUE(q)     (q->ticks)
#include "priority_queue.h"
#undef QUEUE_TYPE
#undef QUEUE_SIZE
#undef ELEMENT_TYPE
#undef ELEMENT_COPY
#undef ELEMENT_VALUE
