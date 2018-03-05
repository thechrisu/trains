#include "router.h"

#define INFINITE_TICKS 1E8

void update_search_node(search_node_queue *q, search_node *current, int direction, uint32_t velocity) {
  track_edge *edge_in_direction = &current->node->edge[direction];
  search_node *node_in_direction = search_node_queue_find_by_node(q, edge_in_direction->dest);
  if (node_in_direction != NULL_SEARCH_NODE) {
    int updated_ticks = current->ticks + edge_in_direction->dist * 10000 / velocity;
    if (updated_ticks < node_in_direction->ticks) {
      node_in_direction->ticks = updated_ticks;
      node_in_direction->prev = current;
    }
  }
}

bool plan_route(track_state *t, int train, location *start, location *end, uint32_t velocity, int current_time, reservation route[MAX_ROUTE_LENGTH]) {
  track_node *start_node = find_sensor(t, start->sensor);
  track_node *end_node = find_sensor(t, end->sensor);

  search_node_queue q;
  search_node_queue_init(&q);

  for (int i = 0; i < TRACK_MAX; i += 1) {
    search_node node;
    track_node *t_node = &(t->track[i]);
    node.node = t_node;
    node.ticks = t_node == start_node ? current_time : INFINITE_TICKS;
    node.prev = NULL_SEARCH_NODE;
    search_node_queue_enqueue(&q, &node);
  }

  search_node dequeued_nodes[TRACK_MAX];
  int i = 0;
  while (search_node_queue_peek(&q) != NULL_SEARCH_NODE) {
    search_node *current = &dequeued_nodes[i];
    search_node_queue_dequeue(&q, current);

    track_node *t_node = current->node;
    switch (t_node->type) {
      case NODE_SENSOR:
      case NODE_MERGE:
      case NODE_ENTER:
        update_search_node(&q, current, DIR_AHEAD, velocity);
        search_node_queue_heapify(&q);
        break;
      case NODE_BRANCH:
        update_search_node(&q, current, DIR_STRAIGHT, velocity);
        update_search_node(&q, current, DIR_CURVED, velocity);
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
  for (int i = 0; i < TRACK_MAX; i += 1) {
    search_node *n = &dequeued_nodes[i];
    if (n->node == end_node) {
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

  route[path_length + 1].train = 0;
  current = end_node_after_search;
  for (int i = path_length; i >= 0; i -= 1) {
    route[i].train = train;
    route[i].node = current->node;

    // Time in ticks to travel 30 cm at current velocity
    // 30 * 10 * 100 mm/100 / velocity mm/100/s * 100 ticks/s
    uint32_t train_length_travel_time = 30 * 10 * 100 * 100 / velocity;
    route[i].ticks_start = current->ticks - train_length_travel_time;
    route[i].ticks_end = current->ticks + train_length_travel_time;

    current = current->prev;
  }

  return true;
}

int route_length(reservation route[MAX_ROUTE_LENGTH]) {
  int result = 0;
  while (result < MAX_ROUTE_LENGTH && route[result].train != 0) {
    result += 1;
  }
  return result;
}

void router() {
  RegisterAs("Router");

  int clock_server_tid = WhoIs("ClockServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  bool has_made_reservation[80];
  for (int i = 0; i < 80; i += 1) {
    has_made_reservation[i] = false;
  }

  reservation reservations[80][MAX_ROUTE_LENGTH];
  for (int i = 0; i < 80; i += 1) {
    for (int j = 0; j < MAX_ROUTE_LENGTH; j += 1) {
      reservations[i][j].train = 0;
    }
  }

  int sender_tid;
  message send, received, reply;

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
    switch (received.type) {
      case MESSAGE_GET_ROUTE: {
        int train = received.msg.get_route_params.train;
        Assert(train > 0 && train <= 80);

        if (has_made_reservation[train]) {
          reply.type = REPLY_GET_ROUTE_EXISTING_ROUTE;
        } else {
          int speed = received.msg.get_route_params.speed;
          location *start = &received.msg.get_route_params.start;
          location *end = &received.msg.get_route_params.end;

          send.type = MESSAGE_GETCONSTANTSPEEDMODEL;
          send.msg.train = train;
          Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

          uint32_t velocity = reply.msg.train_speeds[speed];
          int current_time = Time(clock_server_tid);

          reservation forwards[MAX_ROUTE_LENGTH];
          bool forwards_success = plan_route(&track, train, start, end, velocity, current_time, forwards);

          location end_backwards;
          location_reverse(&track, &end_backwards, end);

          reservation backwards[MAX_ROUTE_LENGTH];
          bool backwards_success = plan_route(&track, train, start, &end_backwards, velocity, current_time, backwards);

          if (!forwards_success && !backwards_success) {
            reply.type = REPLY_GET_ROUTE_ERROR;
          } else {
            if (forwards_success && backwards_success) {
              tmemcpy(reservations[train],
                      route_length(forwards) < route_length(backwards) ? forwards : backwards,
                      MAX_ROUTE_LENGTH * sizeof(reservation));
            } else {
              tmemcpy(reservations[train],
                      forwards_success ? forwards : backwards,
                      MAX_ROUTE_LENGTH * sizeof(reservation));
            }

            has_made_reservation[train] = true;
            reply.type = REPLY_GET_ROUTE_OK;
            reply.msg.route = reservations[train];
          }
        }

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      case MESSAGE_CANCEL_ROUTE: {
        int train = received.msg.train;
        Assert(train > 0 && train <= 80);

        if (has_made_reservation[train]) {
          has_made_reservation[train] = false;
          reply.type = REPLY_CANCEL_ROUTE_OK;
        } else {
          reply.type = REPLY_CANCEL_ROUTE_NO_ROUTE;
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

int get_route(int train, int speed, location *start, location *end, reservation route[MAX_ROUTE_LENGTH]) {
  message send, reply;
  send.type = MESSAGE_GET_ROUTE;
  send.msg.get_route_params.train = train;
  send.msg.get_route_params.speed = speed;
  send.msg.get_route_params.start.sensor = start->sensor;
  send.msg.get_route_params.start.offset = start->offset;
  send.msg.get_route_params.end.sensor = end->sensor;
  send.msg.get_route_params.end.offset = end->offset;
  Assert(Send(WhoIs("Router"), &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

  switch (reply.type) {
    case REPLY_GET_ROUTE_OK:
      tmemcpy(route, reply.msg.route, MAX_ROUTE_LENGTH * sizeof(reservation));
      return 0;
    case REPLY_GET_ROUTE_EXISTING_ROUTE:
      return -1;
    case REPLY_GET_ROUTE_ERROR:
      return -2;
    default:
      logprintf("Router responded to get request with message of type %d\n\r", reply.type);
      Assert(0);
      return 0;
  }
}

int cancel_route(int train) {
  message send, reply;
  send.type = MESSAGE_CANCEL_ROUTE;
  send.msg.train = train;
  Assert(Send(WhoIs("Router"), &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

  switch (reply.type) {
    case REPLY_CANCEL_ROUTE_OK:
      return 0;
    case REPLY_CANCEL_ROUTE_NO_ROUTE:
      return -1;
    default:
      logprintf("Router responded to cancel request with message of type %d\n\r", reply.type);
      Assert(0);
      return 0;
  }
}
