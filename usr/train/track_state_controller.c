#include "track_state_controller.h"

void track_state_controller() {
  Assert(RegisterAs("TrackStateController") == 0);
  int sender_tid;
  int train;
  message received, reply;
  track_state track;
  init_track(&track);
  int16_t sensor_states[10];
  tmemset(sensor_states, 0, sizeof(sensor_states));

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    switch (received.type) {
      case MESSAGE_SENSORSRECEIVED:
        for (int i = 0; i < 10; i++) {
          sensor_states[i] = received.msg.sensors[i];
          logprintf("TSC Sensor %d: %x\n\r", i + 1, sensor_states[i]);
        }
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      case MESSAGE_GETTRAIN:
        train = received.msg.train;
        Assert(train >= 0 && train <= 80);
        reply.type = REPLY_GETTRAIN;
        reply.msg.tr_data.direction = track.train[train].direction;
        reply.msg.tr_data.should_speed = track.train[train].should_speed;
        reply.msg.tr_data.headlights = track.train[train].headlights;
        Reply(sender_tid, &reply, sizeof(reply));
        break;
      case MESSAGE_GETSENSORS:
        reply.type = REPLY_GETSENSORS;
        for (int i = 0; i < 10; i++) {
          reply.msg.sensors[i] = sensor_states[i];
        }
        Reply(sender_tid, &reply, sizeof(reply));
        break;
      case MESSAGE_TRAINSETSPEED:
        train = received.msg.train;
        Assert(train >= 0 && train <= 80);
        Assert(received.msg.tr_data.should_speed >= 0
               && received.msg.tr_data.should_speed <= 14);
        track.train[train].should_speed = received.msg.tr_data.should_speed;
#if DEBUG_REVERSAL
        logprintf("Track state controller: Set speed of %d to %d\n\r", train, track.train[train].should_speed);
#endif /* DEBUG_REVERSAL */
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      case MESSAGE_TRAINREVERSED:
        train = received.msg.train;
        Assert(train >= 0 && train <= 80);
        track.train[train].direction = received.msg.tr_data.direction;
        track.train[train].should_speed = received.msg.tr_data.should_speed;
        Assert(received.msg.tr_data.should_speed >= 0
               && received.msg.tr_data.should_speed <= 14);
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      default:
        logprintf("track_state_controller received message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
