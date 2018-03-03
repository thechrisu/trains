#include "router.h"

void plan_route(location *start, location *end, uint32_t velocity, int current_time, reservation reservations[MAX_RESERVATIONS], reservation *reservation_lists[TRACK_MAX], reservation route[MAX_ROUTE_LENGTH]) {
  (void)start;
  (void)end;
  (void)velocity;
  (void)current_time;
  (void)reservations;
  (void)reservation_lists;
  (void)route;

  // Do Dijkstra's
  // Walk backwards along shortest path, creating reservations and adding them to lists
}

void router() {
  RegisterAs("Router");

  int clock_server_tid = WhoIs("ClockServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  reservation reservations[MAX_RESERVATIONS];
  for (int i = 0; i < MAX_RESERVATIONS; i += 1) {
    reservations[i].train = 0;
  }

  bool has_made_reservation[80];
  for (int i = 0; i < 80; i += 1) {
    has_made_reservation[i] = false;
  }

  reservation *reservation_lists[TRACK_MAX];
  for (int i = 0; i < TRACK_MAX; i += 1) {
    reservation_lists[i] = NULL_RESERVATION;
  }

  int sender_tid;
  message send, received, reply;

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
    switch (received.type) {
      case MESSAGE_GET_ROUTE: {
        int train = received.msg.train;
        Assert(train > 0 && train <= 80);

        if (has_made_reservation[train]) {
          reply.type = REPLY_GET_ROUTE_EXISTING_ROUTE;
        } else {
          location *start = &received.msg.get_route_params.start;
          location *end = &received.msg.get_route_params.end;
          reservation route[MAX_ROUTE_LENGTH];

          send.type = MESSAGE_GETTRAIN;
          send.msg.tr_data.train = train;
          Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

          int speed = reply.msg.tr_data.should_speed;

          send.type = MESSAGE_GETCONSTANTSPEEDMODEL;
          send.msg.train = train;
          Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

          uint32_t velocity = reply.msg.train_speeds[speed];
          int current_time = Time(clock_server_tid);
          plan_route(start, end, velocity, current_time, reservations, reservation_lists, route);

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
          // TODO remove reservations
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
