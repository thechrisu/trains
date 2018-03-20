#include "train_util.h"

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]) {
  for (int i = 0; i < 10; i++) {
    leading_edge[i] = ~old_sensors[i] & new_sensors[i]; // 0 -> 1
    old_sensors[i] = new_sensors[i];
  }
}

bool is_sensor_triggered(int16_t sensors[10], unsigned int offset) {
  return sensors[sensor_data_element(offset)] & sensor_data_mask(offset);
}

bool any_sensor_is_triggered(int16_t sensors[10]) {
  for (int i = 0; i < 10; i += 1) {
    if (sensors[i] != 0) {
      return true;
    }
  }

  return false;
}

int poll_until_sensor_triggered_with_timeout(int clock_server_tid,
                                             int track_state_controller_tid,
                                             unsigned int offset,
                                             int timeout_ticks) {
  message reply;
  int16_t current_sensors[10], leading_edge[10];

  for (int i = 0; i < 10; i += 1) {
    current_sensors[i] = 0;
  }

  int current_ticks = 0;

  do {
    Assert(Delay(clock_server_tid, REFRESH_PERIOD) == 0);
    get_sensors(track_state_controller_tid, &reply);
    get_leading_edge(current_sensors, reply.msg.sensors, leading_edge);
    current_ticks += REFRESH_PERIOD;
  } while (!is_sensor_triggered(leading_edge, offset)
             && !(current_ticks > timeout_ticks));
  return current_ticks > timeout_ticks;
}

bool poll_until_sensor_pair_triggered_with_timeout(int clock_server_tid,
                                                   int track_state_controller_tid,
                                                   unsigned int offset,
                                                   int timeout_ticks) {
  message reply;
  int16_t current_sensors[10], leading_edge[10];

  for (int i = 0; i < 10; i += 1) {
    current_sensors[i] = 0;
  }

  int current_ticks = 0;

  do {
    Assert(Delay(clock_server_tid, REFRESH_PERIOD) == 0);
    get_sensors(track_state_controller_tid, &reply);
    get_leading_edge(current_sensors, reply.msg.sensors, leading_edge);
    current_ticks += REFRESH_PERIOD;
  } while (!is_sensor_triggered(leading_edge, offset) &&
           !is_sensor_triggered(leading_edge, sensor_pair(&track, offset)) &&
           current_ticks <= timeout_ticks);
  return current_ticks > timeout_ticks;
}

void poll_until_sensor_triggered(int clock_server_tid,
                                 int track_state_controller_tid,
                                 unsigned int offset) {
  poll_until_sensor_triggered_with_timeout(clock_server_tid,
                                           track_state_controller_tid,
                                           offset, 100 * 60 * 20);
}

void poll_until_at_dist(int clock_server, int terminal_tx_server,
                  int dist, int velocity) {
  int last_stopping = Time(clock_server);
  int start_dist = dist;
  while (dist > 0) {
    Delay(clock_server, REFRESH_PERIOD);
    int diff = dist_from_last_sensor(clock_server, last_stopping, velocity);
    dist = start_dist - diff;
    if (terminal_tx_server > 0)
      Assert(Printf(terminal_tx_server, "%s%d;%dH(%d left - step %d)%s", ESC,
        CALIB_LINE + 2, 1, dist, diff, HIDE_CURSOR_TO_EOL) == 0);
  }
}

// TODO maybe do this via switchers?
void switch_turnouts_within_distance(int clock_server, int train_tx_server,
                                     int track_state_controller,
                                     track_node *route[MAX_ROUTE_LENGTH],
                                     location *loc,
                                     int distance) {
  turnout_state turnout_states[NUM_TURNOUTS];
  get_turnouts(track_state_controller, turnout_states);

  bool passed_loc = false;

  for (track_node **c = route; *(c + 1) != NULL_TRACK_NODE; c += 1) {
    if ((*c)->type == NODE_SENSOR && (*c)->num == (int)loc->sensor) {
      passed_loc = true;
    }

    if (passed_loc) {
      if (get_dist_on_route(route, loc, c) > distance) {
        return;
      }

      if ((*c)->type == NODE_BRANCH) {
        int map_offset = turnout_num_to_map_offset((*c)->num);
        if (*(c + 1) == STRAIGHT(*c) && turnout_states[map_offset] == TURNOUT_CURVED) {
          switch_turnout(clock_server, train_tx_server, track_state_controller, (*c)->num, false);
        } else if (*(c + 1) == CURVED(*c) && turnout_states[map_offset] == TURNOUT_STRAIGHT) {
          switch_turnout(clock_server, train_tx_server, track_state_controller, (*c)->num, true);
        } else if (*(c + 1) != STRAIGHT(*c) && *(c + 1) != CURVED(*c)) {
          logprintf("Route has switch nodes %d -> %d, but they're not connected\n\r",
                    (*c)->num, (*(c + 1))->num);
        }
      }
    }
  }
}

float get_fudged_stopping_distance_factor(int train) {
  return 0.33333
    * ((track.stopping_distance[train][12] / track.stopping_distance[74][12])
    + (track.stopping_distance[train][13] / track.stopping_distance[74][13])
    + (track.stopping_distance[train][14] / track.stopping_distance[74][14])
    );
}

int stopping_dist_from_velocity(int velocity, uint32_t velocity_model[15],
                                uint32_t stopping_distance_model[15]) {
  int smaller = -1;
  int larger = -1;
  for (int i = 0; i <= 14; i++) {
    if ((int)velocity_model[i] < velocity) {
      smaller = i;
    }
    if ((int)velocity_model[i] > velocity) {
      larger = i;
      break;
    }
    if ((int)velocity_model[i] == velocity) {
      return (int)stopping_distance_model[i];
    }
  }
  if (smaller == -1) return 0;
  int smaller_val = (int)stopping_distance_model[smaller];
  int larger_val = (int)stopping_distance_model[larger == -1 ? 14 : larger];
  int diff = larger_val - smaller_val;
  float r;
#if ACC_CALIB_DEBUG
  logprintf("smaller speed: %d, larger speed: %d\n\r", smaller, larger);
#endif /* ACC_CALIB_DEBUG */
  if (larger == -1) {
    r = 1.0;
  } else {
    int numerator = velocity - (int)velocity_model[smaller];
    int denominator = (int)velocity_model[larger] - (int)velocity_model[smaller];
    r = (float)numerator / (float)denominator;
#if ACC_CALIB_DEBUG
    logprintf("num: %d, float: %d\n\r", numerator, denominator);
    logprintf("velo: %d, smaller: %d, larger: %d\n\r", velocity, velocity_model[smaller], velocity_model[larger]);
#endif /* ACC_CALIB_DEBUG */
  }
#if ACC_CALIB_DEBUG
  logprintf("Smaller: %d, larger: %d, diff: %d, r: %d\n\r", smaller_val, larger_val, diff, (int)(r * 1000));
#endif /* ACC_CALIB_DEBUG */
  Assert((int)(smaller_val + diff * r) < 200000);
  Assert(r >= 0.0 && r <= 1.0);
  return smaller_val + diff * r;
}

int stopping_dist_remaining_dist(int train, int speed, int ticks_left) {
  return 0.5 + (1 / get_fudged_stopping_distance_factor(train)) * (speed *  6 * 0.80619 - 2 * 5.47489) * ticks_left * ticks_left * 0.5;
}

bool on_route(track_node *route[MAX_ROUTE_LENGTH], location *loc) {
  for (track_node **c = route; *c != NULL_TRACK_NODE; c += 1) {
    if ((*c)->type == NODE_SENSOR && (*c)->num == (int)loc->sensor) {
      return true;
    }
  }
  return false;
}

int get_remaining_dist_in_route(track_node *route[MAX_ROUTE_LENGTH], location *loc) {
  int dist_remaining_100th_mm = 0;
  bool passed_loc = false;

  for (track_node **c = route; *(c + 1) != NULL_TRACK_NODE; c += 1) {
    if ((*c)->type == NODE_SENSOR && (*c)->num == (int)loc->sensor) {
      passed_loc = true;
    }

    if (passed_loc) {
      switch ((*c)->type) {
        case NODE_SENSOR:
        case NODE_MERGE:
        case NODE_ENTER:
          dist_remaining_100th_mm += (*c)->edge[DIR_AHEAD].dist * 100;
          break;
        case NODE_BRANCH:
          if (*(c + 1) == STRAIGHT(*c)) {
            dist_remaining_100th_mm += (*c)->edge[DIR_STRAIGHT].dist * 100;
          } else {
            dist_remaining_100th_mm += (*c)->edge[DIR_CURVED].dist * 100;
          }
          break;
        default:
          logprintf("Invalid node type when getting remaining distance of route: %d\n\r", (*c)->type);
          break;
      }
    }
  }

  return dist_remaining_100th_mm - loc->offset;
}

track_node **get_next_of_type(track_node **remaining_route, node_type type) {
  for (track_node **c = remaining_route + 1; *c != NULL_TRACK_NODE; c += 1) {
    if ((*c)->type == type) {
      return c;
    }
  }
  return (track_node **)0;
}

int get_dist_on_route(track_node *route[MAX_ROUTE_LENGTH], location *loc, track_node **end) {
  int dist_remaining_100th_mm = 0;
  bool passed_loc = false;

  for (track_node **c = route; *c != NULL_TRACK_NODE && c != end; c += 1) {
    if ((*c)->type == NODE_SENSOR && (*c)->num == (int)loc->sensor) {
      passed_loc = true;
    }

    if (passed_loc) {
      switch ((*c)->type) {
        case NODE_SENSOR:
        case NODE_MERGE:
        case NODE_ENTER:
          dist_remaining_100th_mm += (*c)->edge[DIR_AHEAD].dist * 100;
          break;
        case NODE_BRANCH:
          if (*(c + 1) == STRAIGHT(*c)) {
            dist_remaining_100th_mm += (*c)->edge[DIR_STRAIGHT].dist * 100;
          } else {
            dist_remaining_100th_mm += (*c)->edge[DIR_CURVED].dist * 100;
          }
          break;
        default:
          logprintf("Invalid node type when getting distance between nodes: %d\n\r", (*c)->type);
          break;
      }
    }
  }
  return dist_remaining_100th_mm - loc->offset;
}

bool is_reverse_in_distance(track_node *route[MAX_ROUTE_LENGTH], location *loc,
                            int distance) {
  bool passed_loc = false;

  track_node *last_switch = NULL_TRACK_NODE;
  for (track_node **c = route; *(c + 1) != NULL_TRACK_NODE; c += 1) {
    if ((*c)->type == NODE_SENSOR && (*c)->num == (int)loc->sensor) {
      passed_loc = true;
    }

    if (passed_loc) {
      if (get_dist_on_route(route, loc, c) > distance) return last_switch != NULL_TRACK_NODE;
      if ((*c)->type == NODE_MERGE
          && *(c + 1) != NULL_TRACK_NODE && (*(c + 1))->type == NODE_BRANCH
          && *(c + 2) != NULL_TRACK_NODE && (*(c + 1))->type == NODE_MERGE) {
        track_node *other_dir = STRAIGHT(*(c + 1)) == *c ?
                                        CURVED(*(c + 1)) : STRAIGHT(*(c + 1));
        if (other_dir == *(c + 2)) {
          last_switch = *(c + 1);
        }
      }
    }
  }
  return last_switch != NULL_TRACK_NODE;
}
