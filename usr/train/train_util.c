#include "train_util.h"

void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]) {
  for (int i = 0; i  < 10; i++) {
    leading_edge[i] = ~old_sensors[i] & new_sensors[i]; // 0 -> 1
    old_sensors[i] = new_sensors[i];
  }
}

bool is_sensor_triggered(int16_t sensors[10], unsigned int offset) {
  return sensors[sensor_data_element(offset)] & sensor_data_mask(offset);
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

void route_switch_turnouts(int clock_server, int train_tx_server,
                           int track_state_controller, reservation *route) {
  reservation *c = route;
  while ((c + 1)->train != 0) {
    if (c->node->type == NODE_BRANCH) {
      if ((c + 1)->node == STRAIGHT(c->node)) {
        switch_turnout(clock_server, train_tx_server, track_state_controller, c->node->num, false);
      } else if ((c + 1)->node == CURVED(c->node)) {
        switch_turnout(clock_server, train_tx_server, track_state_controller, c->node->num, true);
      } else {
        logprintf("Route has switch nodes %d -> %d, but they're not connected\n\r",
                  c->node->num, (c + 1)->node->num);
      }
    }
    c += 1;
  }
}

float get_fudged_stopping_distance_factor(int train) {
  return 0.33333
    * ((track.stopping_distance[train][12] / track.stopping_distance[74][12])
    + (track.stopping_distance[train][13] / track.stopping_distance[74][13])
    + (track.stopping_distance[train][14] / track.stopping_distance[74][14])
    );
}

int stopping_dist_remaining_dist(int train, int speed, int ticks_left) {
  return 0.5 + (1 / get_fudged_stopping_distance_factor(train)) * (speed *  6 * 0.80619 - 2 * 5.47489) * ticks_left * ticks_left * 0.5;
}

int get_remaining_dist_in_route(reservation *remaining_route) {
  int dist_remaining_100th_mm = 0;
  reservation *c = remaining_route;
  while ((c + 1)->train != 0) {
    switch (c->node->type) {
      case NODE_SENSOR:
      case NODE_MERGE:
      case NODE_ENTER:
        dist_remaining_100th_mm += c->node->edge[DIR_AHEAD].dist * 100;
        break;
      case NODE_BRANCH:
        if ((c + 1)->node == STRAIGHT(c->node)) {
          dist_remaining_100th_mm += c->node->edge[DIR_STRAIGHT].dist * 100;
        } else {
          dist_remaining_100th_mm += c->node->edge[DIR_CURVED].dist * 100;
        }
        break;
      default:
        logprintf("Invalid node type when getting remaining distance of route: %d\n\r", c->node->type);
        break;
    }
    c += 1;
  }
  return dist_remaining_100th_mm;
}

reservation *get_next_of_type(reservation *remaining_route, node_type type) {
  reservation *c = remaining_route + 1;
  while (c->train != 0) {
    if (c->node->type == type) {
      return c;
    }
    c += 1;
  }
  return NULL_RESERVATION;
}

int get_dist_between_reservations(reservation *start, reservation *end) {
  int dist_remaining_100th_mm = 0;
  reservation *c = start;
  while ((c + 1)->train != 0 || c != end) {
    switch (c->node->type) {
      case NODE_SENSOR:
      case NODE_MERGE:
      case NODE_ENTER:
        dist_remaining_100th_mm += c->node->edge[DIR_AHEAD].dist * 100;
        break;
      case NODE_BRANCH:
        if ((c + 1)->node == STRAIGHT(c->node)) {
          dist_remaining_100th_mm += c->node->edge[DIR_STRAIGHT].dist * 100;
        } else {
          dist_remaining_100th_mm += c->node->edge[DIR_CURVED].dist * 100;
        }
        break;
      default:
        logprintf("Invalid node type when getting distance between nodes: %d\n\r", c->node->type);
        break;
    }
    c += 1;
  }
  return dist_remaining_100th_mm;
}

enum train_mode get_train_mode(int track_state_controller_tid, int train,
                            uint32_t stopping_times[15]) {
  train_data tr_data;
  get_train(track_state_controller_tid, train, &tr_data);
  if (tr_data.time_speed_last_changed > (int)stopping_times[tr_data.should_speed] / (1000 * 10)
        || tr_data.should_speed == tr_data.last_speed) {
    return CONSTANT_VELOCITY;
  } else if (tr_data.should_speed > tr_data.last_speed) {
    return DECELERATING;
  } else {
    return ACCELERATING;
  }
}
