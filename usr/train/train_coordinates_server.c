#include "train_coordinates_server.h"

void update_coordinates_helper(int now_ticks,
                               turnout_state turnout_states[NUM_TURNOUTS],
                               coordinates *c) {
  int current_velocity = c->velocity + c->acceleration * (now_ticks - c->ticks) / 100;
  if ((c->acceleration < 0 && current_velocity < c->target_velocity) ||
      (c->acceleration > 0 && current_velocity > c->target_velocity)) {
    current_velocity = c->target_velocity;
  }

  if (c->loc.sensor != NO_NEXT_SENSOR) {
    int velocity_diff = current_velocity - c->velocity;
    c->loc.offset += current_velocity * (now_ticks - c->ticks) / 100 -
                     (velocity_diff * velocity_diff) / (2 * c->acceleration);
    location_canonicalize(&track, turnout_states, &c->loc, &c->loc);
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

  c->loc.sensor = last_sensor_hit->sensor;
  c->loc.offset = 0;
}

// TODO account for delay in sending command to train?
void update_coordinates_after_speed_change(train_data *tr_data,
                                           uint32_t velocity_model[15],
                                           int acceleration,
                                           turnout_state turnout_states[NUM_TURNOUTS],
                                           coordinates *c) {
  update_coordinates_helper(tr_data->time_speed_last_changed, turnout_states, c);

  c->current_speed = tr_data->should_speed;
  c->last_speed = tr_data->last_speed;

  c->target_velocity = velocity_model[tr_data->should_speed];
  c->acceleration = acceleration;
}

void update_coordinates_after_reverse(coordinates *c) {
  if (c->loc.sensor != NO_NEXT_SENSOR) {
    location_reverse(&track, &c->loc, &c->loc);
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

  coordinates coords[81];
  for (int i = 0; i <= 80; i += 1) {
    coords[i].loc.sensor = NO_NEXT_SENSOR;
  }

  turnout_state turnout_states[NUM_TURNOUTS];

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    message_update_coords *uc = &received.msg.update_coords;
    coordinates *train_coords = &coords[(int)uc->tr_data.train];
    switch (received.type) {
      case MESSAGE_UPDATE_COORDS_SPEED:
        update_coordinates_after_speed_change(&uc->tr_data, uc->velocity_model,
                                              uc->acceleration, turnout_states,
                                              train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_UPDATE_COORDS_REVERSE:
        update_coordinates_after_reverse(train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_UPDATE_COORDS_SENSOR:
        update_coordinates_after_sensor_hit(&uc->last_sensor, turnout_states,
                                            train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_FORWARD_TURNOUT_STATES:
        tmemcpy(turnout_states, &received.msg.turnout_states,
                NUM_TURNOUTS * sizeof(turnout_state));
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_GET_COORDINATES: {
        coordinates *c = &coords[(int)received.msg.train];
        update_coordinates_after_time_passed(clock_server, turnout_states, c);
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
