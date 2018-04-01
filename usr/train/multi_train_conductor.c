#include "multi_train_conductor.h"

#define CONDUCTOR_STOP_CHECK_INTERVAL 2
#define CONDUCTOR_SENSOR_CHECK_INTERVAL 1

#define SWITCH_LOOKAHEAD_NODES 10

#define SWITCH_DIST 25000

#define ABS(a) ((a) > 0 ? (a) : (-(a)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

void multi_conductor_setspeed(int train_tx_server, int track_state_controller,
                              train_group *group, int speed) {
  // TODO implement this for real
  // 1. Get grouped trains' velocity models
  // 2. Pick speeds for trains to follow at
  // 3. If one follower or more can't oscillate around given speed, try with a lower speed
  // 4. Change trains' speeds to picked speeds

  (void)train_tx_server;
  (void)track_state_controller;
  (void)group;
  (void)speed;
}

void multi_conductor_reverse_to_speed(int train_tx_server,
                                int track_state_controller, int clock_server,
                                int train, int speed) {
  stop_and_reverse_train_to_speed(clock_server, train_tx_server,
                                  track_state_controller, train, speed);
  // TODO implement this for real
}

void multi_train_conductor() {
  int sender_tid;
  message received, ready;
  int train_tx_server = WhoIs("TrainTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int cmd_dispatcher = WhoIs("CommandDispatcher");
  int train_coordinates_server = WhoIs("TrainCoordinatesServer");
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);
  Assert(cmd_dispatcher > 0);
  Assert(train_coordinates_server > 0);

  train_group g;

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
  Assert(received.type == MESSAGE_MULTICONDUCTOR_SETGROUP);
  tmemcpy(&g, &received.msg.group_content, sizeof(g));
  ready.type = MESSAGE_READY;
  bool is_done = false;

  int coordinate_courier_tid = -1;
  while (!is_done) {
    if (coordinate_courier_tid < 0) {
      coordinate_courier_tid = create_multi_courier(&g);
    }
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    switch (received.type) {
      case MESSAGE_SUNSET:
        Kill(coordinate_courier_tid);
        is_done = true;
        break;
      case MESSAGE_USER:
        switch (received.msg.cmd.type) {
          case USER_CMD_LOOP:
            // TODO
            break;
          case USER_CMD_TRG: {
            multi_conductor_setspeed(train_tx_server,
                                     track_state_controller,
                                     &g, received.msg.cmd.data[1]);
            break;
          }
          case USER_CMD_RV:
            break;
          case USER_CMD_R:
            Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
            break; // TODO
          default:
            logprintf("Got user cmd message of type %d\n\r", received.msg.cmd.type);
            Assert(0);
            break;
        }
        Assert(Send(cmd_dispatcher, &ready, sizeof(ready), EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_CONDUCTOR_NOTIFY_REQUEST:
        switch (received.msg.notification_response.reason) {
          case GOT_LOST:
          case LOCATION_CHANGED:
          case LOCATION_ANY: {
            message next_req;
            next_req.type = REPLY_CONDUCTOR_NOTIFY_REQUEST;
            next_req.msg.notification_request.drop_existing = true;
            next_req.msg.notification_request.num_requests = 0;
            Assert(Reply(sender_tid, &next_req, sizeof(next_req)) == 0);
            break;
          }
          case SPACING:
            Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
            break;
          default:
            logprintf("Multitrainconductor: Notification of unknown type %d\n\r",
                received.msg.notification_response.reason);
            Assert(0 && "Multitrainconductor: Notification of unknown type");
            break;
        }
        break;
      default:
        logprintf("Got user cmd message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
