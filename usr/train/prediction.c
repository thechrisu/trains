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
                        int train, unsigned int sensor,
                        coordinates *prediction) {
  (void)train_coordinates_server_tid;
  (void)turnout_states;
  (void)train;
  (void)sensor;
  (void)prediction;
}
