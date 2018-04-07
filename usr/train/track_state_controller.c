#include "track_state_controller.h"

track_state track;

/**
 * Sends the latest velocity model/train state of one train
 * to the train coordinates server.
 *
 * @param train                    Train to update
 * @param clock_server_tid         Clock server task id
 * @param train_coords_server_tid  Train coordinate server task id
 */
void track_controller_update_coordinates(int train, int clock_server_tid,
                                         int train_coords_server_tid) {
  message send;

  send.type = MESSAGE_UPDATE_COORDS_SPEED;
  tmemcpy(&send.msg.update_coords.tr_data, &track.train[train], sizeof(train_data));
  send.msg.update_coords.tr_data.train = train;
  send.msg.update_coords.tr_data.time_speed_last_changed = Time(clock_server_tid);
  tmemcpy(&send.msg.update_coords.velocity_model,
          track.speed_to_velocity[train],
          15 * sizeof(uint32_t));
  tmemcpy(&send.msg.update_coords.stopping_distance_model,
          track.stopping_distance[train],
          15 * sizeof(uint32_t));

  Assert(Send(train_coords_server_tid,
              &send, sizeof(send),
              EMPTY_MESSAGE, 0) == 0);
}

void track_state_controller() {
  Assert(RegisterAs("TrackStateController") == 0);
  int sender_tid;
  int train;
  message send, received, reply;
  init_track(&track);
  int16_t sensor_states[10];
  tmemset(sensor_states, 0, sizeof(sensor_states));

  int clock_server_tid = WhoIs("ClockServer");
  int train_coords_server_tid = Create(MyPriority(), &train_coordinates_server);
  Assert(train_coords_server_tid > 0);

  int event_server = WhoIs("EventServer");
  Assert(event_server > 0);

  Subscribe(event_server, EVENT_SENSOR_DATA_RECEIVED);

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    switch (received.type) {
      case MESSAGE_EVENT:
        switch (received.msg.event.type) {
          case EVENT_SENSOR_DATA_RECEIVED:
            for (int i = 0; i < 10; i++) {
              sensor_states[i] = received.msg.event.body.sensors[i];
            }
            Reply(sender_tid, EMPTY_MESSAGE, 0);
            break;
          default:
            logprintf("Unknown event type %d in track state controller\n\r",
                      received.msg.event.type);
            Assert(0);
            break;
        }
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
      case MESSAGE_TRAINSETDIRECTION: {
        train = received.msg.tr_data.train;
        track.train[train].direction = received.msg.tr_data.direction;
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      }
      case MESSAGE_TRAINSETSPEED: {
        train = received.msg.tr_data.train;
        Assert(train >= 0 && train <= 80);
        Assert(received.msg.tr_data.should_speed >= 0
               && received.msg.tr_data.should_speed <= 14);

        track.train[train].headlights = received.msg.tr_data.headlights;

        if (received.msg.tr_data.should_speed != track.train[train].should_speed) {
          track.train[train].last_speed = track.train[train].should_speed;
          track.train[train].should_speed = received.msg.tr_data.should_speed;
          track.train[train].time_speed_last_changed = Time(clock_server_tid);

          track_controller_update_coordinates(train, clock_server_tid,
                                              train_coords_server_tid);
        }
#if DEBUG_REVERSAL
        logprintf("Track state controller: Set speed of %d to %d\n\r", train, track.train[train].should_speed);
#endif /* DEBUG_REVERSAL */
        Reply(sender_tid, EMPTY_MESSAGE, 0);
        break;
      }
      case MESSAGE_TRAINREVERSED:
        train = received.msg.tr_data.train;
        Assert(train >= 0 && train <= 80);
        track.train[train].direction = received.msg.tr_data.direction;
        Reply(sender_tid, EMPTY_MESSAGE, 0);

        send.type = MESSAGE_UPDATE_COORDS_REVERSE;
        send.msg.tr_data.train = train;
        Assert(Send(train_coords_server_tid,
                    &send, sizeof(send),
                    EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_TURNOUTSWITCHED: {
        int turnout_num = received.msg.turnout_switched_params.turnout_num;
        Assert(is_valid_turnout_num(turnout_num));
        track.turnouts[turnout_num_to_map_offset(turnout_num)] = received.msg.turnout_switched_params.state;
        Reply(sender_tid, EMPTY_MESSAGE, 0);

        send.type = MESSAGE_FORWARD_TURNOUT_STATES;
        tmemcpy(&send.msg.turnout_states, &track.turnouts,
                NUM_TURNOUTS * sizeof(turnout_state));
        Assert(Send(train_coords_server_tid,
               &send, sizeof(send),
               EMPTY_MESSAGE, 0) == 0);
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
        uint32_t distance = distance_between_track_nodes(find_sensor(&track, start), find_sensor(&track, end));

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
        if (velocity >= DEFINITE_MAX_CM_PER_SEC * 10 * 100) {
          logprintf("Got a velocity of %d 1/100 mm/s in track state controller\n\r", velocity);
        } else {
          if (track.speed_to_velocity[t][s] == 0) {
            track.speed_to_velocity[t][s] = velocity;
          } else {
            track.speed_to_velocity[t][s] *= 9;
            track.speed_to_velocity[t][s] += velocity;
            track.speed_to_velocity[t][s] /= 10;
          }
        }
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      case MESSAGE_GETSTOPPINGDISTANCEMODEL: {
        int t = received.msg.train;
        Assert(t >= 1 && t <= 80);
        reply.type = REPLY_GETSTOPPINGDISTANCEMODEL;
        for (int i = 0; i < 15; i++) {
          reply.msg.train_distances[i] = track.stopping_distance[t][i];
        }
        Reply(sender_tid, &reply, sizeof(reply));
        break;
      }
      case MESSAGE_UPDATESTOPPINGDISTANCEMODEL: {
        int t = received.msg.usdm.train;
        int s = received.msg.usdm.speed;
        uint32_t v = received.msg.usdm.value;
        Assert(t >= 1 && t <= 80);
        Assert(s >= 0 && s <= 14);
        if (v < 200000) {
          if (track.stopping_distance[t][s] == 0) {
            track.stopping_distance[t][s] = v;
          } else {
            track.stopping_distance[t][s] *= 9;
            track.stopping_distance[t][s] += v;
            track.stopping_distance[t][s] /= 10;
          }
        }
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      case MESSAGE_GETSTOPPINGTIMEMODEL: {
        int t = received.msg.train;
        Assert(t >= 1 && t <= 80);
        reply.type = REPLY_GETSTOPPINGTIMEMODEL;
        for (int i = 0; i < 15; i++) {
          reply.msg.train_times[i] = track.stopping_time_mus[t][i];
        }
        Reply(sender_tid, &reply, sizeof(reply));
        break;
      }
      case MESSAGE_UPDATESTOPPINGTIMEMODEL: {
        int t = received.msg.ustm.train;
        int s = received.msg.ustm.speed;
        uint32_t v = received.msg.ustm.value;
        Assert(t >= 1 && t <= 80);
        Assert(s >= 0 && s <= 14);
        if (v < 1000 * 1000 * 10) {
          if (track.stopping_time_mus[t][s] == 0) {
            track.stopping_time_mus[t][s] = v;
          } else {
            track.stopping_time_mus[t][s] *= 9;
            track.stopping_time_mus[t][s] += v;
            track.stopping_time_mus[t][s] /= 10;
          }
        }
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      case MESSAGE_REQUEST_COORD_UPDATE: {
        Reply(sender_tid, EMPTY_MESSAGE, 0);

        track_controller_update_coordinates(received.msg.train,
                                            clock_server_tid,
                                            train_coords_server_tid);
        break;
      }
      default:
        logprintf("track_state_controller received message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
