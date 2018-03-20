#include "track_reservation_server.h"

#define NOT_RESERVED 5678

#define RESPONSE_OK                0
#define RESPONSE_NO_SUCH_EDGE     -1
#define RESPONSE_NOT_RESERVED     -2
#define RESPONSE_ALREADY_RESERVED -2

static int reserved_by[TRACK_MAX][TRACK_MAX];

int handle_reservation_request(message_reservation_request *req,
                                int check_for_reservation_by,
                                int reserve_for,
                                int reservation_error_response) {
  track_node *start = req->nodes[0];
  track_node *end = req->nodes[1];

  if (!node_follows(start, end)) {
    return RESPONSE_NO_SUCH_EDGE;
  } else {
    int start_index = node_index_in_track_state(&track, start);
    int end_index = node_index_in_track_state(&track, end);

    int reverse_start_index = node_index_in_track_state(&track, end->reverse);
    int reverse_end_index = node_index_in_track_state(&track, start->reverse);

    if (reserved_by[start_index][end_index] == check_for_reservation_by) {
      if (reserved_by[reverse_start_index][reverse_end_index] != check_for_reservation_by) {
        logprintf("Edge from %s to %s was owned by %d, but reverse from %s to %s was not",
                  start->name, end->name, check_for_reservation_by,
                  start->reverse->name, end->reverse->name);
        return reservation_error_response;
      }

      reserved_by[start_index][end_index] = reserve_for;
      reserved_by[reverse_start_index][reverse_end_index] = reserve_for;
      return RESPONSE_OK;
    } else {
      return reservation_error_response;
    }
  }
}


void track_reservation_server() {
  Assert(RegisterAs("TrackReservationServer") == 0);

  int sender_tid;
  message received, reply;

  for (int i = 0; i < TRACK_MAX; i += 1) {
    for (int j = 0; j < TRACK_MAX; j += 1) {
      reserved_by[i][j] = NOT_RESERVED;
    }
  }

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    switch (received.type) {
      case MESSAGE_RESERVATION_MAKE: {
        reply.type = REPLY_RESERVATION_MAKE;

        int response = handle_reservation_request(&received.msg.reservation_request,
                                                  NOT_RESERVED,
                                                  received.msg.reservation_request.train,
                                                  RESPONSE_ALREADY_RESERVED);

        reply.msg.reservation_response = response;

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      case MESSAGE_RESERVATION_DROP: {
        reply.type = REPLY_RESERVATION_DROP;

        int response = handle_reservation_request(&received.msg.reservation_request,
                                                  received.msg.reservation_request.train,
                                                  NOT_RESERVED,
                                                  RESPONSE_NOT_RESERVED);

        reply.msg.reservation_response = response;

        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      case MESSAGE_RESERVATION_DROP_ALL: {
        int train = received.msg.reservation_request.train;

        for (int i = 0; i < TRACK_MAX; i += 1) {
          for (int j = 0; j < TRACK_MAX; j += 1) {
            if (reserved_by[i][j] == train) {
              reserved_by[i][j] = NOT_RESERVED;
            }
          }
        }

        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      default:
        logprintf("Received message of type %d in track reservation server\n\r", received.type);
        Assert(0);
    }
  }
}
