/**
 * @file
 * @brief Data structures describing the track/train state
 */
#ifndef TRACK_NODE_H
#define TRACK_NODE_H

#include "tstdlib.h"
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

struct track_edge {
  struct track_edge *reverse;
  struct track_node *src, *dest;
  int dist;             /* in millimetres */
};

typedef struct track_edge track_edge;

struct track_node {
  const char *name;
  node_type type;
  int num;              /* sensor or switch number */
  struct track_node *reverse;  /* same location, but opposite direction */
  track_edge edge[2];
};

typedef struct track_node track_node;

typedef struct {
  char train;
  bool direction; // true/positive: forward
  int should_speed;
  bool headlights;
  int last_speed;
  int time_speed_last_changed;
} train_data;

typedef enum {
  TURNOUT_STRAIGHT,
  TURNOUT_CURVED,
  TURNOUT_UNKNOWN
} turnout_state;

typedef struct {
  turnout_state turnouts[NUM_TURNOUTS]; // offset 18-21 map to 153-156
  train_data train[81];
  track_node track[TRACK_MAX];
  uint32_t speed_to_velocity[81][15]; // 1 here is 1/100 mm/s
  uint32_t stopping_distance[81][15]; // 1 here is 1/100 mm
  uint32_t stopping_time_mus[81][15]; // 1 here is 1 microsecond
} track_state;

typedef struct {
  unsigned int sensor;
  int32_t offset; // 1 here is 1/100 mm. Positive is in the same direction as the sensor.
} location;

struct res {
  int train;
  track_node *node;
  int ticks_start;
  int ticks_end;
  struct res *next;
  struct res *prev;
};
typedef struct res reservation;

#define NULL_RESERVATION (reservation *)0

#endif /* TRACK_NODE_H */
