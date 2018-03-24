#include "prediction.h"

int dist_from_last_sensor(int clock_server, int ticks_at_last_sensor,
                          uint32_t velocity) {
  int c_time = Time(clock_server);
  return (int)velocity * (c_time - ticks_at_last_sensor) / 100;
}

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

  track_node *next_track_node = track_node_next(current.loc.node, turnout_states);

  if (next_track_node == NULL_TRACK_NODE) {
    prediction->loc.node = NULL_TRACK_NODE;
    prediction->ticks = INFINITE_TICKS;
    return;
  }

  prediction->loc.node = next_track_node;
  prediction->loc.offset = 0;
  int dist_to_next_track_node = 100 * distance_between_track_nodes(current.loc.node,
                                                                   next_track_node) - current.loc.offset;

  if (current.velocity == 0) {
    prediction->ticks = INFINITE_TICKS;
  } else if (current.target_velocity == 0) {
    long long current_velocity = current.velocity;
    int max_distance_before_stop = (int)((current_velocity * current_velocity) /
                                         (2 * -current.acceleration));

    if (max_distance_before_stop < dist_to_next_track_node) {
      prediction->ticks = INFINITE_TICKS;
    } else {
      long long to_sqrt = (current_velocity * current_velocity) +
                          2 * current.acceleration * dist_to_next_track_node;
      int sqrt_result = (int)(1.0F / tinvsqrt((float)to_sqrt));
      prediction->ticks = current.ticks + (100 * (sqrt_result - current_velocity)) /
                                          current.acceleration;
    }
  } else if (current.acceleration == 0) {
    prediction->ticks = current.ticks + 100 * dist_to_next_track_node / current.velocity;
  } else {
    long long velocity_diff = current.velocity - current.target_velocity;
    long long ticks_to_next_track_node = 100 * dist_to_next_track_node / current.target_velocity +
                                         100 * (velocity_diff * velocity_diff) /
                                         (2 * current.acceleration * current.target_velocity);
    prediction->ticks = current.ticks + (int)ticks_to_next_track_node;
  }

}
