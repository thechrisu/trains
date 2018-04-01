#include "multi_train_conductor.h"

#define CONDUCTOR_STOP_CHECK_INTERVAL 2
#define CONDUCTOR_SENSOR_CHECK_INTERVAL 1

#define SWITCH_LOOKAHEAD_NODES 10

#define SWITCH_DIST 25000

#define ABS(a) ((a) > 0 ? (a) : (-(a)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

/**
 * @param   target_velocity The velocity to target.
 * @param   velocities      The train's possible velocities.
 * @returns The speed that gives a velocity just above the target velocity,
 *          or -1 if none exists.
 */
int speed_above(uint32_t target_velocity, uint32_t velocities[15]) {
  for (int i = 0; i <= 14; i += 1) {
    if (velocities[i] >= target_velocity) {
      return i;
    }
  }

  return -1;
}

/**
 * @param   target_velocity The velocity to target.
 * @param   velocities      The train's possible velocities.
 * @returns The speed that gives a velocity just below the target velocity,
 *          or -1 if none exists.
 */
int speed_below(uint32_t target_velocity, uint32_t velocities[15]) {
  for (int i = 14; i >= 0; i -= 1) {
    if (velocities[i] <= target_velocity) {
      return i;
    }
  }

  return -1;
}

void multi_conductor_setspeed(int train_tx_server, int track_state_controller,
                              train_group *group, int speed) {
  message velocity_model[group->num_members];
  for (int i = 0; i < group->num_members; i += 1) {
    get_constant_velocity_model(track_state_controller, group->members[i],
                                &velocity_model[i]);
  }

  int speeds[group->num_members];
  speeds[0] = speed;
  for (int i = 1; i < group->num_members; i += 1) {
    speeds[i] = -1;
  }

  bool found_speed = true;

  do {
    uint32_t lead_velocity = velocity_model[0].msg.train_speeds[speeds[0]];

    for (int i = 1; i < group->num_members; i += 1) {
      if (velocity_model[i].msg.train_speeds[14] < lead_velocity) {
        found_speed = false;
        break;
      } else {
        speeds[i] = speed_below(lead_velocity, velocity_model[i].msg.train_speeds);
      }
    }

    if (!found_speed) {
      speeds[0] -= 1;
      found_speed = true;
    }
  } while (!found_speed);

  for (int i = 0; i < group->num_members; i += 1) {
    Assert(speeds[i] >= 0);
    set_train_speed(train_tx_server, track_state_controller,
                    group->members[0], speeds[i]);
  }
}

void reverse_group(int train_tx_server, int track_state_controller,
                   train_group *group) {

  train_group group_copy;
  tmemcpy(&group_copy, group, sizeof(group_copy));
  for (int i = 0; i < group->num_members; i++) {
    reverse_train(train_tx_server, track_state_controller, group->members[i]);
  }
  int j = 0;
  for (int i = group_copy.num_members - 1; i >= 0; i--, j++) {
    group->members[i] = group_copy.members[j];
  }
  Assert(group->num_members == group_copy.num_members);
}

void multi_train_conductor() {
  int sender_tid;
  message received;
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
  bool is_done = false;

  int coordinate_courier_tid = -1;

  train_data tr_data; // Needed to reverse to previous speed
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
          case USER_CMD_TRG:
            multi_conductor_setspeed(train_tx_server,
                                     track_state_controller,
                                     &g, received.msg.cmd.data[1]);
            Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
            break;
          case USER_CMD_RVG:
            get_train(track_state_controller, g.members[0], &tr_data);
            set_alarm(tr_data.should_speed * 33);
            Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
            break;
          default:
            logprintf("Got user cmd message of type %d\n\r", received.msg.cmd.type);
            Assert(0);
            break;
        }
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
          case SPACING: {
            int leader = received.msg.notification_response.subject.trains[0];
            int follower = received.msg.notification_response.subject.trains[1];

            int actual_distance = received.msg.notification_response.action.distance[0];
            int expected_distance = received.msg.notification_response.action.distance[1];

            int new_speed;

            coordinates leader_coords, follower_coords;

            get_coordinates(train_coordinates_server, leader, &leader_coords);
            get_coordinates(train_coordinates_server, follower, &follower_coords);

            if (leader_coords.velocity == 0 && leader_coords.target_velocity == 0) {
              new_speed = 0;
            } else {
              message leader_velocity_model, follower_velocity_model;

              get_constant_velocity_model(track_state_controller, leader,
                                          &leader_velocity_model);
              get_constant_velocity_model(track_state_controller, follower,
                                          &follower_velocity_model);

              if (actual_distance < expected_distance) {
                new_speed = leader_coords.acceleration > 0 ?
                            speed_above(follower_coords.velocity,
                                        follower_velocity_model.msg.train_speeds) :
                            speed_below(leader_coords.target_velocity,
                                        follower_velocity_model.msg.train_speeds);
              } else if (actual_distance > expected_distance) {
                new_speed = leader_coords.acceleration < 0 ?
                            speed_below(follower_coords.velocity,
                                        follower_velocity_model.msg.train_speeds) :
                            speed_above(leader_coords.target_velocity,
                                        follower_velocity_model.msg.train_speeds);

                // If the follower can't catch up to the leader's current velocity,
                // reduce the velocity of the first train in the group to make sure
                // the follower can catch up in the future.
                if (new_speed == -1) {
                  message first_velocity_model;
                  get_constant_velocity_model(track_state_controller, g.members[0],
                                              &first_velocity_model);

                  int first_target_speed = speed_below(follower_velocity_model.msg.train_speeds[14],
                                                       first_velocity_model.msg.train_speeds);
                  if (first_target_speed != -1) {
                    set_train_speed(train_tx_server, track_state_controller,
                                    g.members[0], first_target_speed);
                  }
                }
              } else {
                new_speed = -1;
              }
            }

            if (new_speed != -1) {
              set_train_speed(train_tx_server, track_state_controller,
                              follower, new_speed);
            }

            Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
            break;
          } // SPACING
          default:
            logprintf("Multitrainconductor: Notification of unknown type %d\n\r",
                received.msg.notification_response.reason);
            Assert(0 && "Multitrainconductor: Notification of unknown type");
            break;
        } // MESSAGE_CONDUCTOR_NOTIFY_REQUEST
        break;
      case MESSAGE_WAKEUP: { // GOOD MORNING
        reverse_group(train_tx_server, track_state_controller, &g); // speed 15 + reorder group
        Kill(coordinate_courier_tid);
        Delay(clock_server, 8); // HACK
        coordinate_courier_tid = create_multi_courier(&g);
        multi_conductor_setspeed(train_tx_server, track_state_controller, &g,
                                 tr_data.should_speed);
        break;
      }
      default:
        logprintf("Got user cmd message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
