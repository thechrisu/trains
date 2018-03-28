#include "router.h"

#define INFINITE_DISTANCE 1E8

void update_search_node(search_node_queue *q, search_node *current, int direction) {
  track_edge *edge_in_direction = &current->node->edge[direction];
  search_node *node_in_direction = search_node_queue_find_by_node(q, edge_in_direction->dest);
  if (node_in_direction != NULL_SEARCH_NODE) {
    int updated_distance = current->distance + edge_in_direction->dist;
    if (updated_distance < node_in_direction->distance) {
      node_in_direction->distance = updated_distance;
      node_in_direction->prev = current;
    }
  }
}

bool plan_route(track_state *t, location *start, location *end, track_node *route[MAX_ROUTE_LENGTH]) {
  search_node_queue q;
  search_node_queue_init(&q);

  for (int i = 0; i < t->nodes; i += 1) {
    search_node node;
    track_node *t_node = &(t->track[i]);
    node.node = t_node;
    node.distance = t_node == start->node ? 0 : INFINITE_DISTANCE;
    node.prev = NULL_SEARCH_NODE;
    search_node_queue_enqueue(&q, &node);
  }

  search_node dequeued_nodes[t->nodes];
  int i = 0;
  while (search_node_queue_peek(&q) != NULL_SEARCH_NODE) {
    search_node *current = &dequeued_nodes[i];
    search_node_queue_dequeue(&q, current);

    track_node *t_node = current->node;
    switch (t_node->type) {
      case NODE_SENSOR:
      case NODE_MERGE:
      case NODE_ENTER:
        update_search_node(&q, current, DIR_AHEAD);
        search_node_queue_heapify(&q);
        break;
      case NODE_BRANCH:
        update_search_node(&q, current, DIR_STRAIGHT);
        update_search_node(&q, current, DIR_CURVED);
        search_node_queue_heapify(&q);
        break;
      case NODE_EXIT:
        break;
      default:
        logprintf("Unknown node type %d in plan_route\n\r", t_node->type);
        break;
    }

    i += 1;
  }

  search_node *end_node_after_search = NULL_SEARCH_NODE;
  for (int i = 0; i < t->nodes; i += 1) {
    search_node *n = &dequeued_nodes[i];
    if (n->node == end->node) {
      end_node_after_search = n;
    }
  }

  if (end_node_after_search == NULL_SEARCH_NODE ||
      (end_node_after_search->prev == NULL_SEARCH_NODE &&
       end_node_after_search != &dequeued_nodes[0])) {
    return false;
  }

  int path_length = 0;
  search_node *current = end_node_after_search;
  while (current != &dequeued_nodes[0]) {
    path_length += 1;
    current = current->prev;
  }

  route[path_length + 1] = NULL_TRACK_NODE;
  current = end_node_after_search;
  for (int i = path_length; i >= 0; i -= 1) {
    route[i] = current->node;
    current = current->prev;
  }

  return true;
}

int route_node_count(track_node *route[MAX_ROUTE_LENGTH]) {
  int result;
  for (result = 0; route[result] != NULL_TRACK_NODE; result += 1);
  return result;
}

int route_length(track_node *route[MAX_ROUTE_LENGTH]) {
  location loc;
  loc.node = route[0];
  loc.offset = 0;
  return get_remaining_dist_in_route(route, &loc);
}

void router() {
  RegisterAs("Router");

  int sender_tid;
  message received, reply;

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
    switch (received.type) {
      case MESSAGE_GET_ROUTE: {
        location *start = &received.msg.get_route_params.start;
        location *end = &received.msg.get_route_params.end;
        track_node **route = received.msg.get_route_params.route;

        track_node *forwards[MAX_ROUTE_LENGTH];
        bool forwards_success = plan_route(&track, start, end, forwards);

        location end_backwards;
        location_reverse(&end_backwards, end);

        track_node *backwards[MAX_ROUTE_LENGTH];
        bool backwards_success = plan_route(&track, start, &end_backwards, backwards);

        if (!forwards_success && !backwards_success) {
          reply.type = REPLY_GET_ROUTE_ERROR;
        } else {
          if (forwards_success && backwards_success) {
            tmemcpy(route,
                    route_length(forwards) < route_length(backwards) ? forwards : backwards,
                    MAX_ROUTE_LENGTH * sizeof(track_node *));
          } else {
            tmemcpy(route,
                    forwards_success ? forwards : backwards,
                    MAX_ROUTE_LENGTH * sizeof(track_node *));
          }

          reply.type = REPLY_GET_ROUTE_OK;
        }

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      default:
        logprintf("Router got message of type %d\n\r", received.type);
        Assert(0);
    }
  }

  logprintf("Router exited\n\r");
  Assert(0);
}

int get_route(location *start, location *end, track_node *route[MAX_ROUTE_LENGTH]) {
  message send, received;

  send.type = MESSAGE_GET_ROUTE;
  send.msg.get_route_params.start.node = start->node;
  send.msg.get_route_params.start.offset = start->offset;
  send.msg.get_route_params.end.node = end->node;
  send.msg.get_route_params.end.offset = end->offset;
  send.msg.get_route_params.route = route;
  Assert(Send(WhoIs("Router"), &send, sizeof(send),
                               &received, sizeof(received)) == sizeof(received));

  switch (received.type) {
    case REPLY_GET_ROUTE_OK:
      return 0;
    case REPLY_GET_ROUTE_ERROR:
      return -1;
    default:
      logprintf("Router responded to get request with message of type %d\n\r", received.type);
      Assert(0);
      return 0;
  }
}
