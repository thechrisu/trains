#include "search_node_queue.h"

void search_node_copy(search_node *from, search_node *to) {
  to->node = from->node;
  to->distance = from->distance;
  to->prev = from->prev;
}

#define QUEUE_TYPE           search_node_queue
#define QUEUE_SIZE           SEARCH_NODE_QUEUE_SIZE
#define ELEMENT_TYPE         search_node
#define ELEMENT_COPY         search_node_copy
#define ELEMENT_VALUE(q)     (q->distance)
#include "priority_queue.h"
#undef QUEUE_TYPE
#undef QUEUE_SIZE
#undef ELEMENT_TYPE
#undef ELEMENT_COPY
#undef ELEMENT_VALUE

search_node *search_node_queue_find_by_node(search_node_queue *q, track_node *tn) {
  for (unsigned int i = 0; i < q->size; i += 1) {
    search_node *current = &(q->elts[i]);
    if (current->node == tn) {
      return current;
    }
  }
  return NULL_SEARCH_NODE;
}
