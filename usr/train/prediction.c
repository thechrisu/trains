#include "prediction.h"

int dist_from_last_sensor(int clock_server, int ticks_at_last_sensor,
                          uint32_t velocity) {
  int c_time = Time(clock_server);
  return (int)velocity * (c_time - ticks_at_last_sensor) / 100;
}

void get_location_from_last_sensor_hit(int clock_server, int velocity,
                                       reply_get_last_sensor_hit *last_record,
                                       location *current) {
  current->sensor = last_record->sensor;
  current->offset = dist_from_last_sensor(clock_server, last_record->ticks,
                                      velocity);
}

void predict_sensor_hit(int train_coordinates_server_tid,
                        turnout_state turnout_states[NUM_TURNOUTS],
                        int train, coordinates *prediction) {
  coordinates current;
  get_coordinates(train_coordinates_server_tid, train, &current);

  if (current.loc.sensor == NO_NEXT_SENSOR) {
    prediction->loc.sensor = NO_NEXT_SENSOR;
    return;
  }

  unsigned int next_sensor = sensor_next(&track, current.loc.sensor, turnout_states);

  prediction->loc.sensor = next_sensor;
  prediction->loc.offset = 0;

  int dist_to_next_sensor = 100 * distance_between_sensors(&track,
                                                           current.loc.sensor,
                                                           next_sensor) - current.loc.offset;

  if (current.target_velocity == 0) {
    int max_distance_before_stop = current.velocity / (2 * current.acceleration);

    if (max_distance_before_stop < dist_to_next_sensor) {
      prediction->ticks = INFINITE_TICKS;
    } else {
      prediction->ticks = current.ticks + (2 * dist_to_next_sensor) /
                                          (current.velocity + current.target_velocity);
    }
  } else {
    long long velocity_diff = current.velocity - current.target_velocity;
    long long ticks_to_next_sensor = dist_to_next_sensor / current.target_velocity +
                                     (velocity_diff * velocity_diff) /
                                     (2 * current.acceleration * current.target_velocity);
    prediction->ticks = current.ticks + (int)ticks_to_next_sensor;
  }

}
