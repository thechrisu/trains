/**
 * @file
 * @brief Concrete priority queue implementation for track graph search
 */
#ifndef SEARCH_NODE_QUEUE_H
#define SEARCH_NODE_QUEUE_H

#include "track_node.h"
#include "track_state_controller.h"
#include "tstdlib.h"

/**
 * A `search_node` associates a track node with a time and a pointer to the previous
 * node in a shortest path.
 */
struct sn {
  track_node *node;
  int ticks;
  struct sn *prev;
};
typedef struct sn search_node;

#define NULL_SEARCH_NODE (search_node *)0

#define SEARCH_NODE_QUEUE_SIZE TRACK_MAX

/**
 * A search node queue maintains a list of track nodes, ordered by their distance
 * from the start node in a graph search.
 */
typedef struct {
  uint32_t size;
  search_node elts[SEARCH_NODE_QUEUE_SIZE];
} search_node_queue;

/**
 * Initializes a search node queue.
 *
 * @param q A search node queue.
 */
void search_node_queue_init(search_node_queue *q);

/**
 * Adds a search node record to the given search node queue.
 *
 * @param   q A search node queue.
 * @param   e A search node to add to the queue.
 * @returns -1 if the queue is full and 0 otherwise.
 */
int search_node_queue_enqueue(search_node_queue *q, search_node *e);

/**
 * Removes a search node record from the given search node queue.
 *
 * @param   q A search node queue.
 * @param   e The removed element is copied into this argument.
 * @returns -1 if the queue is empty and 0 otherwise
 */
int search_node_queue_dequeue(search_node_queue *q, search_node *e);

/**
 * Returns a pointer to the first search node record on the queue.
 *
 * @param   q A search node queue.
 * @returns NULL_SEARCH_NODE if the queue is empty and the first record otherwise.
 */
search_node *search_node_queue_peek(search_node_queue *q);

/**
 * Restores the heap invariant in a search node queue.
 *
 * Call this method after modifying the priorities of any of the nodes in the queue.
 * @param q A search node queue.
 */
void search_node_queue_heapify(search_node_queue *q);


/**
 * Finds the search node in a queue that points to
 *
 * @param   q  A search node queue.
 * @param   tn The track node to search by.
 * @returns The search node, or NULL_SEARCH_NODE if no such node could be found.
 */
search_node *search_node_queue_find_by_node(search_node_queue *q, track_node *tn);

#endif /* SEARCH_NODE_QUEUE_H */
