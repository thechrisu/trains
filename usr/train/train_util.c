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
                           int track_state_controller, track_node **route) {
  for (track_node **c = route; *c != (track_node *)0; c += 1) {
    if ((*c)->type == NODE_BRANCH) {
      if (*(c + 1) == STRAIGHT(*c)) {
        switch_turnout(clock_server, train_tx_server, track_state_controller, (*c)->num, false);
      } else if (*(c + 1) == CURVED(*c)) {
        switch_turnout(clock_server, train_tx_server, track_state_controller, (*c)->num, true);
      } else {
        logprintf("Route has switch nodes %d -> %d, but they're not connected\n\r",
                  (*c)->num, (*(c + 1))->num);
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

int stopping_dist_remaining_dist(int train, int speed, int ticks_left) {
  return 0.5 + (1 / get_fudged_stopping_distance_factor(train)) * (speed *  6 * 0.80619 - 2 * 5.47489) * ticks_left * ticks_left * 0.5;
}

int get_remaining_dist_in_route(track_node **remaining_route) {
  int dist_remaining_100th_mm = 0;
  for (track_node **c = remaining_route; *c != (track_node *)0; c += 1) {
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
    c += 1;
  }
  return dist_remaining_100th_mm;
}

track_node **get_next_of_type(track_node **remaining_route, node_type type) {
  for (track_node **c = remaining_route + 1; *c != (track_node *)0; c += 1) {
    if ((*c)->type == type) {
      return c;
    }
  }
  return (track_node **)0;
}

int get_dist_between_reservations(track_node **start, track_node **end) {
  int dist_remaining_100th_mm = 0;
  for (track_node **c = start; *c != (track_node *)0 && c != end; c += 1) {
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
    c += 1;
  }
  return dist_remaining_100th_mm;
}
