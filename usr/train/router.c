#include "router.h"

reservation res[MAX_RESERVATIONS];

void router() {
  RegisterAs("Router");

  for (int i = 0; i < MAX_RESERVATIONS; i += 1) {
    res[i].train = 0;
  }

  bool has_made_reservation[80];
  for (int i = 0; i < 80; i += 1) {
    has_made_reservation[i] = false;
  }

  int sender_tid;
  message received, reply;

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
    switch (received.type) {
      case MESSAGE_GET_ROUTE: {
        int train = received.msg.train;
        Assert(train > 0 && train <= 80);

        if (has_made_reservation[train]) {
          reply.type = REPLY_GET_ROUTE_EXISTING_ROUTE;
        } else {
          reservation route[MAX_ROUTE_LENGTH];

          // TODO find and reserve route
          // Look up velocity of given train
          // Do Dijkstra's with weights as times since start of route
          // If we can't go to a node because it's reserved, increase weight to take into
          // account required waiting time
          // Return the shortest path given these constraints

          has_made_reservation[train] = true;
          reply.type = REPLY_GET_ROUTE_OK;
          reply.msg.route = route;
        }

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == sizeof(reply));
        break;
      }
      case MESSAGE_CANCEL_ROUTE: {
        int train = received.msg.train;
        Assert(train > 0 && train <= 80);

        if (has_made_reservation[train]) {
          for (int i = 0; i < MAX_RESERVATIONS; i += 1) {
            if (res[i].train == train) {
              res[i].train = 0;
            }
          }
          has_made_reservation[train] = false;
          reply.type = REPLY_CANCEL_ROUTE_OK;
        } else {
          reply.type = REPLY_CANCEL_ROUTE_NO_ROUTE;
        }

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == sizeof(reply));
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

int get_route(int train, location *start, location *end, reservation route[MAX_ROUTE_LENGTH]) {
  message send, reply;
  send.type = MESSAGE_GET_ROUTE;
  send.msg.get_route_params.train = train;
  send.msg.get_route_params.start.sensor = start->sensor;
  send.msg.get_route_params.start.offset = start->offset;
  send.msg.get_route_params.end.sensor = end->sensor;
  send.msg.get_route_params.end.offset = end->offset;
  Assert(Send(WhoIs("Router"), &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

  switch (reply.type) {
    case REPLY_GET_ROUTE_OK:
      memcpy(route, reply.msg.route, MAX_ROUTE_LENGTH * sizeof(reservation));
      return 0;
    case REPLY_GET_ROUTE_EXISTING_ROUTE:
      return -1;
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
