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

typedef struct {
  struct track_edge *reverse;
  struct track_node *src, *dest;
  int dist;             /* in millimetres */
} track_edge;

typedef struct {
  const char *name;
  node_type type;
  int num;              /* sensor or switch number */
  struct track_node *reverse;  /* same location, but opposite direction */
  track_edge edge[2];
} track_node;

typedef struct {
  bool direction; // true/positive: forward
  int should_speed;
  bool headlights;
} train_data;

typedef struct {
  char turnouts[NUM_TURNOUTS]; // offset 18-21 map to 153-156
  train_data train[81];
  track_node track[TRACK_MAX];
} track_state;

#endif /* TRACK_NODE_H */
