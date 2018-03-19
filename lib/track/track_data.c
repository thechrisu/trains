#include "track_data.h"

#define FIND_LIMIT 5

void init_track(track_state *global_track) {
  turnout_state *turnouts = global_track->turnouts;

#ifndef TESTING
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
#endif /* TESTING */

  init_tracka(global_track->tracka);
  init_tracka(global_track->trackb);
  global_track->track = global_track->tracka;

#ifndef TESTING
#pragma GCC diagnostic pop
#endif /* TESTING */

  for(int i = 0; i < NUM_TURNOUTS; i++) {
    turnouts[i] = TURNOUT_UNKNOWN; // offset 18-21 map to 153-156
  }
  for(unsigned int i = 0; i < 81; i++) {
    global_track->train[i].should_speed = 0;
  }

  default_value default_speeds[] = {
    { .train = 24, .speed = 0, .value = 0 },
    { .train = 24, .speed = 1, .value = 1073 },
    { .train = 24, .speed = 2, .value = 2332 },
    { .train = 24, .speed = 3, .value = 3780 },
    { .train = 24, .speed = 4, .value = 4448 },
    { .train = 24, .speed = 5, .value = 8308 },
    { .train = 24, .speed = 6, .value = 11652 },
    { .train = 24, .speed = 7, .value = 16502 },
    { .train = 24, .speed = 8, .value = 22005 },
    { .train = 24, .speed = 9, .value = 28094 },
    { .train = 24, .speed = 10, .value = 34369 },
    { .train = 24, .speed = 11, .value = 41091 },
    { .train = 24, .speed = 12, .value = 47540 },
    { .train = 24, .speed = 13, .value = 54614 },
    { .train = 24, .speed = 14, .value = 61857 },
    { .train = 58, .speed = 0, .value = 0 },
    { .train = 58, .speed = 1, .value = 998 },
    { .train = 58, .speed = 2, .value = 5796 },
    { .train = 58, .speed = 3, .value = 8682 },
    { .train = 58, .speed = 4, .value = 12579 },
    { .train = 58, .speed = 5, .value = 13143 },
    { .train = 58, .speed = 6, .value = 15460 },
    { .train = 58, .speed = 7, .value = 27078 },
    { .train = 58, .speed = 8, .value = 30644 },
    { .train = 58, .speed = 9, .value = 37350 },
    { .train = 58, .speed = 10, .value = 30611 },
    { .train = 58, .speed = 11, .value = 40804 },
    { .train = 58, .speed = 12, .value = 44645 },
    { .train = 58, .speed = 13, .value = 49511 },
    { .train = 58, .speed = 14, .value = 57389 },
    { .train = 74, .speed = 0, .value = 0 },
    { .train = 74, .speed = 1, .value = 923 },
    { .train = 74, .speed = 2, .value = 7225 },
    { .train = 74, .speed = 3, .value = 12462 },
    { .train = 74, .speed = 4, .value = 17220 },
    { .train = 74, .speed = 5, .value = 21784 },
    { .train = 74, .speed = 6, .value = 26735 },
    { .train = 74, .speed = 7, .value = 33404 },
    { .train = 74, .speed = 8, .value = 36291 },
    { .train = 74, .speed = 9, .value = 43451 },
    { .train = 74, .speed = 10, .value = 48204 },
    { .train = 74, .speed = 11, .value = 55029 },
    { .train = 74, .speed = 12, .value = 57034 },
    { .train = 74, .speed = 13, .value = 61212 },
    { .train = 74, .speed = 14, .value = 60329 },
    { .train = 1337, .speed = 1337, .value = 1337 }
  };

  uint32_t super_default_speeds[15] = {
    0, 998, 5796, 8682, 12579, 17143, 20990, 27078, 30644, 37350, 40932, 46895, 51378, 57346, 60097,
  };

  default_value default_stopping_distances[] = {
    { .train = 24, .speed = 0, .value = 0 },
    { .train = 24, .speed = 1, .value = 110 },
    { .train = 24, .speed = 2, .value = 350 },
    { .train = 24, .speed = 3, .value = 740 },
    { .train = 24, .speed = 4, .value = 1960 },
    { .train = 24, .speed = 5, .value = 3980 },
    { .train = 24, .speed = 6, .value = 7440 },
    { .train = 24, .speed = 7, .value = 12790 },
    { .train = 24, .speed = 8, .value = 20090 },
    { .train = 24, .speed = 9, .value = 30670 },
    { .train = 24, .speed = 10, .value = 43640 },
    { .train = 24, .speed = 11, .value = 60160 },
    { .train = 24, .speed = 12, .value = 82685 },
    { .train = 24, .speed = 13, .value = 106430 },
    { .train = 24, .speed = 14, .value = 140575 },
    { .train = 58, .speed = 0, .value = 0 },
    { .train = 58, .speed = 1, .value = 110 },
    { .train = 58, .speed = 2, .value = 350 },
    { .train = 58, .speed = 3, .value = 740 },
    { .train = 58, .speed = 4, .value = 1960 },
    { .train = 58, .speed = 5, .value = 3980 },
    { .train = 58, .speed = 6, .value = 7440 },
    { .train = 58, .speed = 7, .value = 12790 },
    { .train = 58, .speed = 8, .value = 20090 },
    { .train = 58, .speed = 9, .value = 30670 },
    { .train = 58, .speed = 10, .value = 43640 },
    { .train = 58, .speed = 11, .value = 60160 },
    { .train = 58, .speed = 12, .value = 74089 },
    { .train = 58, .speed = 13, .value = 93892 },
    { .train = 58, .speed = 14, .value = 143575 },
    { .train = 74, .speed = 0, .value = 0 },
    { .train = 74, .speed = 1, .value = 110 },
    { .train = 74, .speed = 2, .value = 350 },
    { .train = 74, .speed = 3, .value = 740 },
    { .train = 74, .speed = 4, .value = 1960 },
    { .train = 74, .speed = 5, .value = 3980 },
    { .train = 74, .speed = 6, .value = 7440 },
    { .train = 74, .speed = 7, .value = 12790 },
    { .train = 74, .speed = 8, .value = 20090 },
    { .train = 74, .speed = 9, .value = 30670 },
    { .train = 74, .speed = 10, .value = 43640 },
    { .train = 74, .speed = 11, .value = 60160 },
    { .train = 74, .speed = 12, .value = 80220 },
    { .train = 74, .speed = 13, .value = 102330 },
    { .train = 74, .speed = 14, .value = 142342 },
    { .train = 1337, .speed = 1337, .value = 1337 }
  };

  uint32_t super_default_stopping_distances[15] = {
    0, 110, 350, 740, 1960, 3980, 7440, 12790, 20090, 30670, 43640, 60160, 77426, 98431, 139891,
  };

  default_value default_stopping_times[] = {
    { .train = 24, .speed = 0, .value = 0 },
    { .train = 24, .speed = 1, .value = 0 },
    { .train = 24, .speed = 2, .value = 0 },
    { .train = 24, .speed = 3, .value = 0 },
    { .train = 24, .speed = 4, .value = 0 },
    { .train = 24, .speed = 5, .value = 0 },
    { .train = 24, .speed = 6, .value = 0 },
    { .train = 24, .speed = 7, .value = 0 },
    { .train = 24, .speed = 8, .value = 0 },
    { .train = 24, .speed = 9, .value = 0 },
    { .train = 24, .speed = 10, .value = 0 },
    { .train = 24, .speed = 11, .value = 0 },
    { .train = 24, .speed = 12, .value = 2533674 },
    { .train = 24, .speed = 13, .value = 3279754 },
    { .train = 24, .speed = 14, .value = 3430256 },
    { .train = 58, .speed = 0, .value = 0 },
    { .train = 58, .speed = 1, .value = 0 },
    { .train = 58, .speed = 2, .value = 0 },
    { .train = 58, .speed = 3, .value = 0 },
    { .train = 58, .speed = 4, .value = 0 },
    { .train = 58, .speed = 5, .value = 0 },
    { .train = 58, .speed = 6, .value = 0 },
    { .train = 58, .speed = 7, .value = 0 },
    { .train = 58, .speed = 8, .value = 0 },
    { .train = 58, .speed = 9, .value = 0 },
    { .train = 58, .speed = 10, .value = 0 },
    { .train = 58, .speed = 11, .value = 0 },
    { .train = 58, .speed = 12, .value = 2382025 },
    { .train = 58, .speed = 13, .value = 3196000 },
    { .train = 58, .speed = 14, .value = 3344723 },
    { .train = 74, .speed = 0, .value = 0 },
    { .train = 74, .speed = 1, .value = 0 },
    { .train = 74, .speed = 2, .value = 0 },
    { .train = 74, .speed = 3, .value = 0 },
    { .train = 74, .speed = 4, .value = 0 },
    { .train = 74, .speed = 5, .value = 0 },
    { .train = 74, .speed = 6, .value = 0 },
    { .train = 74, .speed = 7, .value = 0 },
    { .train = 74, .speed = 8, .value = 0 },
    { .train = 74, .speed = 9, .value = 0 },
    { .train = 74, .speed = 10, .value = 0 },
    { .train = 74, .speed = 11, .value = 0 },
    { .train = 74, .speed = 12, .value = 0 },
    { .train = 74, .speed = 13, .value = 0 },
    { .train = 74, .speed = 14, .value = 0 },
    { .train = 1337, .speed = 1337, .value = 1337 }
  };

  uint32_t super_default_stopping_times[15] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2710000, 3466000, 3242110,
  };

  setup_default_map(global_track->speed_to_velocity, default_speeds, super_default_speeds);
  setup_default_map(global_track->stopping_distance, default_stopping_distances, super_default_stopping_distances);
  setup_default_map(global_track->stopping_time_mus, default_stopping_times, super_default_stopping_times);
}

unsigned int turnout_num_to_map_offset(unsigned int turnout) {
  if (turnout < 153) return turnout - 1;
  return turnout - (153 - 18);
}

unsigned int map_offset_to_turnout(unsigned int offset) {
  if (offset < 18) return offset + 1;
  return offset + (153 - 18);
}

bool is_valid_turnout_num(unsigned int turnout) {
  return (turnout >= 1 && turnout <= 18) || (turnout >= 153 && turnout <= 156);
}

char sensor_bank(unsigned int offset) {
  return 'A' + offset / 16;
}

unsigned int sensor_index(unsigned int offset) {
  return 1 + offset % 16;
}

unsigned int sensor_data_element(unsigned int offset) {
  return 2 * (sensor_bank(offset) - 'A') + (sensor_index(offset) >= 9 ? 1 : 0);
}

unsigned int sensor_data_mask(unsigned int offset) {
  return 1 << ((16 - sensor_index(offset)) % 8);
}

unsigned int sensor_offset(char bank, unsigned int index) {
  return 16 * (bank - 'A') + (index >= 9 ? 8 : 0) + (index - 1) % 8;
}

track_node *find_sensor(track_state *t, unsigned int offset) {
  char bank = sensor_bank(offset);
  int index = sensor_index(offset);

  char buf[4];
  buf[0] = bank;
  ui2a(index, 10, buf + 1);

  for (int i = 0; i < TRACK_MAX; i += 1) {
    track_node *current_node = &(t->track[i]);
    if (tstrncmp(buf, current_node->name, tstrlen(buf) + 1) == 0) {
      return current_node;
    }
  }

  Assert(0);
  return 0;
}

uint32_t distance_between_sensors_helper(track_node *start, track_node *end,
                                         uint32_t total_distance,
                                         int limit) {
  if (start == end) {
    return total_distance;
  } else if (limit == 0) {
    return 0;
  }

  switch (start->type) {
    case NODE_SENSOR:
    case NODE_MERGE: {
      uint32_t new_total_distance = total_distance + start->edge[DIR_AHEAD].dist;
      return distance_between_sensors_helper(AHEAD(start), end, new_total_distance, limit - 1);
    }
    case NODE_BRANCH: {
      uint32_t straight_total_distance = total_distance + start->edge[DIR_STRAIGHT].dist;
      uint32_t result = distance_between_sensors_helper(STRAIGHT(start), end, straight_total_distance, limit - 1);
      if (result != 0) {
        return result;
      }

      uint32_t curved_total_distance = total_distance + start->edge[DIR_CURVED].dist;
      return distance_between_sensors_helper(CURVED(start), end, curved_total_distance, limit - 1);
    }
    default:
      return 0;
  }
}

uint32_t distance_between_sensors(track_state *t, unsigned int start, unsigned int end) {
  if (start == end) return 0;
  track_node *start_node = find_sensor(t, start);
  track_node *end_node = find_sensor(t, end);
  int result = distance_between_sensors_helper(start_node, end_node, 0, FIND_LIMIT);
  Assert(result != 0);
  return result;
}

uint32_t sensor_is_followed_by_helper(track_node *start, track_node *end, int limit) {
  if (start == end) {
    return true;
  } else if (limit == 0) {
    return false;
  }

  switch (start->type) {
    case NODE_MERGE:
      return sensor_is_followed_by_helper(AHEAD(start), end, limit - 1);
    case NODE_BRANCH: {
      return sensor_is_followed_by_helper(STRAIGHT(start), end, limit - 1) ||
             sensor_is_followed_by_helper(CURVED(start), end, limit - 1);
    }
    default:
      return false;
  }
}

bool sensor_is_followed_by(track_state *t, unsigned int start, unsigned int end) {
  track_node *start_node = find_sensor(t, start);
  return start_node->type == NODE_SENSOR &&
         sensor_is_followed_by_helper(AHEAD(start_node), find_sensor(t, end), FIND_LIMIT);
}

bool sensors_are_paired(track_state *t, unsigned int first, unsigned int second) {
  return find_sensor(t, first)->reverse == find_sensor(t, second);
}

bool sensor_may_be_seen_next_helper(track_node *start, track_node *end, int limit, bool reversed, bool seen_dead_sensor) {
  if (start == end) {
    return true;
  } else if (limit == 0) {
    return false;
  }

  switch (start->type) {
    case NODE_SENSOR:
      return !seen_dead_sensor && (
        sensor_may_be_seen_next_helper(AHEAD(start), end, limit - 1, reversed, true) ||
        (!reversed && sensor_may_be_seen_next_helper(AHEAD(start->reverse), end, limit - 1, true, true))
      );
    case NODE_MERGE:
      return (
        sensor_may_be_seen_next_helper(AHEAD(start), end, limit - 1, reversed, seen_dead_sensor) ||
        (!reversed && sensor_may_be_seen_next_helper(start->reverse, end, limit - 1, true, seen_dead_sensor))
      );
    case NODE_BRANCH:
      return (
        sensor_may_be_seen_next_helper(STRAIGHT(start), end, limit - 1, reversed, seen_dead_sensor) ||
        sensor_may_be_seen_next_helper(CURVED(start), end, limit - 1, reversed, seen_dead_sensor) ||
        (!reversed && sensor_may_be_seen_next_helper(start->reverse, end, limit - 1, true, seen_dead_sensor))
      );
    case NODE_ENTER:
      return sensor_may_be_seen_next_helper(AHEAD(start), end, limit - 1, reversed, seen_dead_sensor);
    case NODE_EXIT:
      return !reversed && sensor_may_be_seen_next_helper(start->reverse, end, limit - 1, true, seen_dead_sensor);
    default:
      return false;
  }
}

bool sensor_may_be_seen_next(track_state *t, unsigned int start, unsigned int end) {
  track_node *start_n = find_sensor(t, start);
  track_node *end_n = find_sensor(t, end);
  return sensor_may_be_seen_next_helper(AHEAD(start_n), end_n, 2 * FIND_LIMIT, false, false) ||
         sensor_may_be_seen_next_helper(AHEAD(start_n->reverse), end_n, 2 * FIND_LIMIT, true, false);
}

bool sensor_reachable_helper(track_node *start, track_node *end, int limit) {
  if (start == end) {
    return true;
  } else if (limit == 0) {
    return false;
  }

  switch (start->type) {
    case NODE_ENTER:
    case NODE_SENSOR:
    case NODE_MERGE:
      return sensor_reachable_helper(AHEAD(start), end, limit - 1);
    case NODE_BRANCH:
      return sensor_reachable_helper(STRAIGHT(start), end, limit - 1) ||
             sensor_reachable_helper(CURVED(start), end, limit - 1);
    default:
      return false;
  }
}

bool sensor_reachable(track_state *t, unsigned int start, unsigned int end) {
  track_node *start_n = find_sensor(t, start);
  track_node *end_n = find_sensor(t, end);
  return start == end || sensor_reachable_helper(AHEAD(start_n), end_n, 50);
}

unsigned int sensor_pair(track_state *t, unsigned int offset) {
  return find_sensor(t, offset)->reverse->num;
}

unsigned int sensor_next(track_state *t, unsigned int start,
                         turnout_state turnout_states[NUM_TURNOUTS]) {
  track_node *current = AHEAD(find_sensor(t, start));

  while (true) {
    switch (current->type) {
      case NODE_SENSOR:
        return current->num;
      case NODE_MERGE:
        current = AHEAD(current);
        break;
      case NODE_BRANCH: {
        unsigned int index = turnout_num_to_map_offset(current->num);
        // Default to curved, since that seems to be a popular initial state
        current = (turnout_states[index] == TURNOUT_STRAIGHT) ? STRAIGHT(current) : CURVED(current);
        break;
      }
      default:
        return NO_NEXT_SENSOR;
    }
  }
}

void location_reverse(track_state *t, location *destination, location *source) {
  destination->sensor = sensor_pair(t, source->sensor);
  destination->offset = -source->offset;
}

void location_canonicalize(track_state *t, turnout_state turnout_states[NUM_TURNOUTS],
                           location *destination, location *source) {
  location current;
  tmemcpy(&current, source, sizeof(current));

  unsigned int next = sensor_next(t, current.sensor, turnout_states);
  if (next != NO_NEXT_SENSOR) {
    int distance_to_next = distance_between_sensors(t, current.sensor, next) * 100;

    while (current.offset >= distance_to_next) {
      current.sensor = next;
      current.offset -= distance_to_next;

      next = sensor_next(t, current.sensor, turnout_states);
      if (next == NO_NEXT_SENSOR) break;
      distance_to_next = distance_between_sensors(t, current.sensor, next) * 100;
    }
  }

  tmemcpy(destination, &current, sizeof(*destination));
}

int distance_diff(track_state *t, turnout_state turnouts[NUM_TURNOUTS],
                  unsigned int sensor, location *loc) {
  if (loc->sensor == NO_NEXT_SENSOR) {
    return 0;
  } else if (loc->sensor == sensor) {
    return loc->offset;
  } else if (sensor == sensor_next(t, loc->sensor, turnouts)) {
    return loc->offset - 100 * distance_between_sensors(t, loc->sensor, sensor);
  }

  return 0;
}
