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
        }
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      case MESSAGE_GETTRAIN:
        train = received.msg.tr_data.train;
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
      case MESSAGE_GETTURNOUTS:
        reply.type = REPLY_GETTURNOUTS;
        for (int i = 0; i < NUM_TURNOUTS; i += 1) {
          reply.msg.turnout_states[i] = track.turnouts[i];
        }
        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      case MESSAGE_TRAINSETSPEED:
        train = received.msg.tr_data.train;
        Assert(train >= 0 && train <= 80);
        Assert(received.msg.tr_data.should_speed >= 0
               && received.msg.tr_data.should_speed <= 14);
        track.train[train].should_speed = received.msg.tr_data.should_speed;
        track.train[train].headlights = received.msg.tr_data.headlights;
#if DEBUG_REVERSAL
        logprintf("Track state controller: Set speed of %d to %d\n\r", train, track.train[train].should_speed);
#endif /* DEBUG_REVERSAL */
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      case MESSAGE_TRAINREVERSED:
        train = received.msg.tr_data.train;
        Assert(train >= 0 && train <= 80);
        track.train[train].direction = received.msg.tr_data.direction;
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      case MESSAGE_TURNOUTSWITCHED: {
        int turnout_num = received.msg.turnout_switched_params.turnout_num;
        Assert(is_valid_turnout_num(turnout_num));
        track.turnouts[turnout_num_to_map_offset(turnout_num)] = received.msg.turnout_switched_params.state;
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      }
      case MESSAGE_GETCONSTANTSPEEDMODEL: {
	int t = received.msg.train;
	Assert(t >= 1 && t <= 80);
        reply.type = REPLY_GETCONSTANTSPEEDMODEL;
        for (int i = 0; i < 15; i++) {
          reply.msg.train_speeds[i] = track.speed_to_velocity[t][i];
        }
        Reply(sender_tid, &reply, sizeof(reply));
	break;
      }
      case MESSAGE_UPDATECONSTANTSPEEDMODEL: {
	int t = received.msg.ucsm.train;
	int s = received.msg.ucsm.speed;
        uint32_t distance = distance_between_sensors(&track, received.msg.ucsm.start, received.msg.ucsm.end);
        uint32_t velocity = distance / received.msg.ucsm.time_elapsed;
	Assert(t >= 1 && t <= 80);
	Assert(s >= 0 && s <= 14);
	Assert(velocity < DEFINITE_MAX_CM_PER_SEC * 10 * 100);
	track.speed_to_velocity[t][s] = velocity;
	Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
	break;
      }
      default:
        logprintf("track_state_controller received message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
