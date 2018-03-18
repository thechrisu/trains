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

  unsigned int next_sensor = sensor_next(&track, current.loc.sensor, turnout_states);

  prediction->loc.sensor = next_sensor;
  prediction->loc.offset = 0;

  if (current.velocity == 0 || next_sensor == NO_NEXT_SENSOR) {
    prediction->ticks = INFINITE_TICKS;
  } else {
    int dist_to_next_sensor = 100 * distance_between_sensors(&track,
                                                             current.loc.sensor,
                                                             next_sensor) - current.loc.offset;
    int velocity_diff = current.velocity - current.target_velocity;
    int ticks_to_next_sensor;

    if (current.acceleration == 0) {
      ticks_to_next_sensor = 100 * dist_to_next_sensor / current.velocity;
    } else {
      int numerator = 2 * current.acceleration * dist_to_next_sensor +
                      velocity_diff * velocity_diff;
      ticks_to_next_sensor = 100 * numerator /
                             (2 * current.acceleration * current.velocity);
    }

    prediction->ticks = current.ticks + ticks_to_next_sensor;
  }
}
