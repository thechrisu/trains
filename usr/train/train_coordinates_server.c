#include "train_coordinates_server.h"

#define MAX_SENSORS_PASSED 3

void update_coordinates_helper(int now_ticks,
                               turnout_state turnout_states[NUM_TURNOUTS],
                               coordinates *c) {
  int current_velocity = c->velocity + c->acceleration * (now_ticks - c->ticks) / 100;
  if ((c->acceleration < 0 && current_velocity < c->target_velocity) ||
      (c->acceleration > 0 && current_velocity > c->target_velocity)) {
    current_velocity = c->target_velocity;
  }

  int velocity_diff = current_velocity - c->velocity;
  c->loc.offset += current_velocity * (now_ticks - c->ticks) / 100;
  if (c->acceleration != 0) {
    c->loc.offset -= (velocity_diff * velocity_diff) / (2 * c->acceleration);
  }

  if (c->loc.node != NULL_TRACK_NODE) {
    location_canonicalize(turnout_states, &c->loc, &c->loc);
  }

  c->velocity = current_velocity;
  if (c->velocity == c->target_velocity) {
    c->acceleration = 0;
  }

  c->ticks = now_ticks;
}

// TODO account for sensor delay?
void update_coordinates_after_sensor_hit(reply_get_last_sensor_hit *last_sensor_hit,
                                         turnout_state turnout_states[NUM_TURNOUTS],
                                         coordinates *c) {
  update_coordinates_helper(last_sensor_hit->ticks, turnout_states, c);

  c->loc.node = find_sensor(&track, last_sensor_hit->sensor);
  c->loc.offset = 0;
}

// TODO account for delay in sending command to train?
void update_coordinates_after_speed_change(train_data *tr_data,
                                           uint32_t velocity_model[15],
                                           uint32_t stopping_distance_model[15],
                                           turnout_state turnout_states[NUM_TURNOUTS],
                                           coordinates *c) {
  update_coordinates_helper(tr_data->time_speed_last_changed, turnout_states, c);

  c->current_speed = tr_data->should_speed;
  c->last_speed = tr_data->last_speed;

  c->target_velocity = velocity_model[tr_data->should_speed];

  // TODO use acceleration model
  long long vel = c->target_velocity == 0 ? c->velocity : c->target_velocity;
  long long sd = stopping_distance_model[c->current_speed == 0 ? c->last_speed : c->current_speed];
  long long accel = (vel * vel) / (2 * sd);

  if (c->velocity < c->target_velocity) {
    c->acceleration = (int)accel;
  } else if (c->velocity == c->target_velocity) {
    c->acceleration = 0;
  } else {
    c->acceleration = -(int)accel;
  }
}

void update_coordinates_after_reverse(coordinates *c) {
  if (c->loc.node != NULL_TRACK_NODE) {
    location_reverse(&c->loc, &c->loc);
    c->loc.offset += PICKUP_LENGTH;
  }
}

void update_coordinates_after_time_passed(int clock_server,
                                          turnout_state turnout_states[NUM_TURNOUTS],
                                          coordinates *c) {
  update_coordinates_helper(Time(clock_server), turnout_states, c);
}

void train_coordinates_server() {
  Assert(RegisterAs("TrainCoordinatesServer") == 0);

  int sender_tid;
  message received, reply;

  int clock_server = WhoIs("ClockServer");

  int event_broker = WhoIs("EventBroker");
  Assert(event_broker > 0);

  Subscribe(event_broker, EVENT_SENSOR_ATTRIBUTED);

  coordinates coords[81];
  unsigned int last_sensor[81];
  int sensors_passed[81];

  for (int i = 0; i <= 80; i += 1) {
    tmemset(&coords[i], 0, sizeof(coords[i]));
    last_sensor[i] = NO_DATA_RECEIVED;
    sensors_passed[i] = 0;
  }

  turnout_state turnout_states[NUM_TURNOUTS];

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    message_update_coords *uc = &received.msg.update_coords;
    int train = uc->tr_data.train;
    coordinates *train_coords = &coords[train];
    switch (received.type) {
      case MESSAGE_UPDATE_COORDS_SPEED:
        update_coordinates_after_speed_change(&uc->tr_data, uc->velocity_model,
                                              uc->stopping_distance_model,
                                              turnout_states,
                                              train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_UPDATE_COORDS_REVERSE:
        update_coordinates_after_reverse(train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_EVENT: {
        Assert(received.msg.event.type == EVENT_SENSOR_ATTRIBUTED);

        reply_get_last_sensor_hit *hit = &received.msg.event.body.attribution.hit;
        train = received.msg.event.body.attribution.train;

        update_coordinates_after_sensor_hit(hit, turnout_states,
                                            &coords[train]);

        last_sensor[train] = hit->sensor;
        sensors_passed[train] = 0;

        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      case MESSAGE_FORWARD_TURNOUT_STATES:
        tmemcpy(turnout_states, &received.msg.turnout_states,
                NUM_TURNOUTS * sizeof(turnout_state));
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_GET_COORDINATES: {
        train = received.msg.train;

        coordinates *c = &coords[train];
        update_coordinates_after_time_passed(clock_server, turnout_states, c);

        if (c->loc.node != NULL_TRACK_NODE &&
            c->loc.node->type == NODE_SENSOR &&
            c->loc.node->num != (int)last_sensor[train]) {
          if (c->loc.node != find_sensor(&track, last_sensor[train])->reverse) {
            sensors_passed[train] += 1;
          }

          last_sensor[train] = c->loc.node->num;

          if (sensors_passed[train] > MAX_SENSORS_PASSED) {
            c->loc.node = NULL_TRACK_NODE;
          }
        }

        reply.type = REPLY_GET_COORDINATES;
        tmemcpy(&(reply.msg.coords), c, sizeof(coordinates));
        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      default:
        logprintf("Received message of type %d in train coordinates server\n\r", received.type);
        Assert(0);
    }
  }
}
