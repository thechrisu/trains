#include "prediction.h"

#define ABS(a) ((a) < 0 ? -(a) : (a))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define ROLLOUT_TICKS 5

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
  int dist_to_next_sensor = 100 * distance_between_track_nodes(current.loc.node,
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

void predict_next_switch(coordinates *co, track_node *route[MAX_ROUTE_LENGTH],
                        coordinates *send_switch_here, int *next_turnout_num,
                        bool *next_is_curved, bool *found, int distance, int max_lookahead) {
  tmemcpy(send_switch_here, co, sizeof(*co));
  int remaining = 0;
  track_node **c;
  *found = false;
  bool passed = false;
  int lookahead = 0;
  for (c = route; *(c + 2) != NULL_TRACK_NODE && lookahead < max_lookahead; c++) {
    if ((*c) == co->loc.node) {
      passed = true;
    }
    if (passed) {
      lookahead++;
    }
    if ((*c)->type == NODE_BRANCH) {
      remaining += (*c)->edge[STRAIGHT(*c) == (*(c + 1)) ? DIR_STRAIGHT : DIR_CURVED].dist * 100;
    } else {
      remaining += (*c)->edge[DIR_AHEAD].dist * 100;
    }
    if (passed && (*(c + 1))->type == NODE_BRANCH) {
      *next_turnout_num = (*(c + 1))->num;
      *next_is_curved = (*(c + 2)) == CURVED(*(c + 1));
      *found = true;
      break;
    }
  }
  if (!(*found)) return;

  send_switch_here->loc.node = (*route);
  send_switch_here->loc.offset = max(0, remaining - distance);
  turnout_state ideal_turnout_states[NUM_TURNOUTS];
  synthesize_turnout_states_from_route(route, ideal_turnout_states);
  location_canonicalize(ideal_turnout_states, &send_switch_here->loc, &send_switch_here->loc);
}

// TODO return total ordering of group instead of just bool
bool will_collide_with_other_train(int distance, coordinates *c, coordinates others[],
                                   int num_other_trains, turnout_state turnout_states[NUM_TURNOUTS],
                                   int *highest_acceptable_speed, int train) {
  coordinates temp;
  tmemcpy(&temp, c, sizeof(temp));
  coordinates one_behind;
  message velocity_model;
  get_constant_velocity_model(WhoIs("TrackStateController"), train, &velocity_model);

  for (int j = c->current_speed; j >= 0; j--) {
    coordinates others_c[num_other_trains];
    tmemcpy(others_c, others, sizeof(others_c));
    tmemcpy(c, &temp, sizeof(temp));
    c->current_speed = j;
    c->target_velocity = velocity_model.msg.train_speeds[j];
    c->acceleration = c->target_velocity - c->velocity;
    Assert(c->loc.offset >= 0);
    int d = 0;
    tmemcpy(&one_behind, c, sizeof(one_behind));
    bool got_collision = false;
    while (d < distance) {
      update_coordinates_helper(c->ticks + ROLLOUT_TICKS,
                                turnout_states, c);
      d += distance_between_locations(&one_behind.loc, &c->loc);
      for (int i = 0; i < num_other_trains; i++) {
        update_coordinates_helper(others_c[i].ticks + ROLLOUT_TICKS,
                                  turnout_states, &others_c[i]);
        int di = distance_between_locations(&c->loc, &others_c[i].loc);
        int di_pair = distance_between_locations(&others_c[i].loc, &c->loc);
        location di_1_loc, di_pair_1_loc;
        location_reverse(&di_1_loc, &c->loc);
        location_reverse(&di_pair_1_loc, &others_c[i].loc);
        int di_1 = distance_between_locations(&di_1_loc, &others_c[i].loc);
        int di_pair_1 = distance_between_locations(&di_pair_1_loc, &c->loc);
        if ((di != -1 && ABS(di) < TRAIN_LENGTH)
                      || (di_1 != -1 && ABS(di_1) < TRAIN_LENGTH)) {
          // bwprintf("We are not ahead.\n\r");
          got_collision = true;
          break;
        }
        if ((di_pair != -1 && ABS(di_pair) < TRAIN_LENGTH)
              || (di_pair_1 != -1 && ABS(di_pair_1) < TRAIN_LENGTH)) {
          // bwprintf("We are ahead.\n\r");
          got_collision = true;
          break;
        }
      }
      if (got_collision) break;
      tmemcpy(&one_behind, c, sizeof(one_behind));
    }
    if (!got_collision) {
      *highest_acceptable_speed = c->current_speed;
      tmemcpy(c, &temp, sizeof(temp));
      return false;
    }
  }
  *highest_acceptable_speed = -1;
  return true;
}
