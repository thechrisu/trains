/**
 * @file
 * @brief Data structures describing the track/train state
 */
#ifndef TRACK_NODE_H
#define TRACK_NODE_H

#include "stdlib.h"
#include "char_buffer.h"

// The track initialization functions expect an array of this size.
#define TRACK_MAX 144
#define NUM_TURNOUTS 22

typedef enum {
  NODE_NONE,
  NODE_SENSOR,
  NODE_BRANCH,
  NODE_MERGE,
  NODE_ENTER,
  NODE_EXIT,
} node_type;

#define DIR_AHEAD 0
#define DIR_STRAIGHT 0
#define DIR_CURVED 1

struct track_node;
typedef struct track_node track_node;
typedef struct track_edge track_edge;

struct track_edge {
  track_edge *reverse;
  track_node *src, *dest;
  int dist;             /* in millimetres */
};

struct track_node {
  const char *name;
  node_type type;
  int num;              /* sensor or switch number */
  track_node *reverse;  /* same location, but opposite direction */
  track_edge edge[2];
};

struct train_data {
  bool direction; // true/positive: forward
  int should_speed;
  bool headlights;
  uint32_t time_reverse_sent;
  bool sent_reverse;
  bool should_restart;
};

struct track_state {
  char turnouts[NUM_TURNOUTS]; // offset 18-21 map to 153-156
  struct train_data train[81];
  struct track_node track[TRACK_MAX];
  bool should_switch_off_solenoid;
  uint32_t last_switch_time;
  uint16_t last_sensor_query;
  char_buffer trains_to_reverse;
  // int train_to_reverse;
};

#endif /* TRACK_NODE_H */
