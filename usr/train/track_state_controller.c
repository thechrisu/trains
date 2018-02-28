#include "track_state_controller.h"

track_state track;

void track_state_controller() {
  Assert(RegisterAs("TrackStateController") == 0);
  int sender_tid;
  int train;
  message received, reply;
  init_track(&track);
  int16_t sensor_states[10];
  tmemset(sensor_states, 0, sizeof(sensor_states));

  int clock_server_tid = WhoIs("ClockServer");

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
        reply.msg.tr_data.last_speed = track.train[train].last_speed;
        reply.msg.tr_data.time_speed_last_changed = track.train[train].time_speed_last_changed;
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
        track.train[train].last_speed = track.train[train].should_speed;
        track.train[train].should_speed = received.msg.tr_data.should_speed;
        track.train[train].headlights = received.msg.tr_data.headlights;
        track.train[train].time_speed_last_changed = Time(clock_server_tid);
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
        int ticks = received.msg.ucsm.ticks;
        unsigned int start = received.msg.ucsm.start;
        unsigned int end = received.msg.ucsm.end;
        uint32_t distance = distance_between_sensors(&track, start, end);

        if (ticks == 0) {
          char start_bank = sensor_bank(start);
          unsigned int start_index = sensor_index(start);
          char end_bank = sensor_bank(end);
          unsigned int end_index = sensor_index(end);
          logprintf("Tried to calibrate velocity between %c%d and %c%d with zero ticks\n\r", start_bank, start_index, end_bank, end_index);
          logprintf("Train: %d, speed: %d, distance: %d\n\r", t, s, distance);
          Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
          break;
        }

        // Distance is in 10^-3 m, one tick is 10^-2 s -> distance per tick is in 10^-1 m/s.
        // We want 10^-5 m/s = 1/100 mm/s -> multiply by 10^4 = 10,000.
        uint32_t velocity = distance * 10000 / received.msg.ucsm.ticks;
        Assert(t >= 1 && t <= 80);
        Assert(s >= 0 && s <= 14);
        Assert(velocity < DEFINITE_MAX_CM_PER_SEC * 10 * 100);
        if (track.speed_to_velocity[t][s] == 0) {
          track.speed_to_velocity[t][s] = velocity;
        } else {
          track.speed_to_velocity[t][s] *= 9;
          track.speed_to_velocity[t][s] += velocity;
          track.speed_to_velocity[t][s] /= 10;
        }
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
