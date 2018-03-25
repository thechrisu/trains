#include "prediction.h"

void predict_sensor_hit(int train_coordinates_server_tid,
                        turnout_state turnout_states[NUM_TURNOUTS],
                        int train, coordinates *prediction) {
  coordinates current;
  get_coordinates(train_coordinates_server_tid, train, &current);

  if (current.loc.node == NULL_TRACK_NODE) {
    prediction->loc.node = NULL_TRACK_NODE;
    prediction->ticks = INFINITE_TICKS;
    return;
  }

  track_node *next_sensor = sensor_next(current.loc.node, turnout_states);

  if (next_sensor == NULL_TRACK_NODE) {
    prediction->loc.node = NULL_TRACK_NODE;
    prediction->ticks = INFINITE_TICKS;
    return;
  }

  prediction->loc.node = next_sensor;
  prediction->loc.offset = 0;
  int dist_to_next_sensor = 100 * distance_between_sensors(current.loc.node,
                                                           next_sensor) - current.loc.offset;

  if (current.velocity == 0) {
    prediction->ticks = INFINITE_TICKS;
  } else if (current.target_velocity == 0) {
    long long current_velocity = current.velocity;
    int max_distance_before_stop = (int)((current_velocity * current_velocity) /
                                         (2 * -current.acceleration));

    if (max_distance_before_stop < dist_to_next_sensor) {
      prediction->ticks = INFINITE_TICKS;
    } else {
      long long to_sqrt = (current_velocity * current_velocity) +
                          2 * current.acceleration * dist_to_next_sensor;
      int sqrt_result = (int)(1.0F / tinvsqrt((float)to_sqrt));
      prediction->ticks = current.ticks + (100 * (sqrt_result - current_velocity)) /
                                          current.acceleration;
    }
  } else if (current.acceleration == 0) {
    prediction->ticks = current.ticks + 100 * dist_to_next_sensor / current.velocity;
  } else {
    long long velocity_diff = current.velocity - current.target_velocity;
    long long ticks_to_next_sensor = 100 * dist_to_next_sensor / current.target_velocity +
                                     100 * (velocity_diff * velocity_diff) /
                                     (2 * current.acceleration * current.target_velocity);
    prediction->ticks = current.ticks + (int)ticks_to_next_sensor;
  }
}

void synthesize_turnout_states_from_route(track_node *route[MAX_ROUTE_LENGTH],
                                          turnout_state states[NUM_TURNOUTS]) {
  for (track_node **c = route; *(c + 1) != NULL_TRACK_NODE; c++) {
    if ((*c)->type == NODE_BRANCH) {
      int turnout_index = turnout_num_to_map_offset((*c)->num);
      if ((*(c + 1)) == STRAIGHT((*c))) {
        states[turnout_index] = TURNOUT_STRAIGHT;
      } else if ((*(c + 1)) == CURVED(*c)) {
        states[turnout_index] = TURNOUT_CURVED;
      }
    }
  }
}

void predict_train_stop(coordinates *c, track_node *route[MAX_ROUTE_LENGTH],
                        coordinates *send_stop_here,
                        uint32_t train_speeds[15], uint32_t train_distances[15]) {
  tmemcpy(send_stop_here, c, sizeof(*c));
  int remaining = get_remaining_dist_in_route(route, &c->loc);
  int stopping_distance = (int)stopping_dist_from_velocity(
                              c->target_velocity, train_speeds, train_distances);
  send_stop_here->loc.offset += remaining - stopping_distance;

  turnout_state ideal_turnout_states[NUM_TURNOUTS];
  synthesize_turnout_states_from_route(route, ideal_turnout_states);
  location_canonicalize(ideal_turnout_states, &send_stop_here->loc, &send_stop_here->loc);
}
