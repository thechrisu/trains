#include "prediction.h"

// TODO measure pickup
#define PICKUP_LENGTH 3 * 10 * 100

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

void update_coordinates_helper(int now_ticks, coordinates *c) {
  int current_velocity = c->velocity + c->acceleration * (now_ticks - c->ticks);
  if ((c->acceleration < 0 && current_velocity < c->target_velocity) ||
      (c->acceleration > 0 && current_velocity > c->target_velocity)) {
    current_velocity = c->target_velocity;
  }

  c->loc.offset += current_velocity * (now_ticks - c->ticks) -
                   ((current_velocity - c->velocity) << 2) / (2 * c->acceleration);
  // TODO canonicalize location

  c->velocity = current_velocity;
  if (c->velocity == c->target_velocity) {
    c->acceleration = 0;
  }

  c->ticks = now_ticks;
}

// TODO account for sensor delay?
void update_coordinates_after_sensor_hit(reply_get_last_sensor_hit *last_sensor_hit,
                                         coordinates *c) {
  update_coordinates_helper(last_sensor_hit->ticks, c);

  c->loc.sensor = last_sensor_hit->sensor;
  c->loc.offset = 0;
}

// TODO account for delay in sending command to train?
void update_coordinates_after_speed_change(train_data *tr_data,
                                           uint32_t velocity_model[15],
                                           int acceleration, coordinates *c) {
  update_coordinates_helper(tr_data->time_speed_last_changed, c);

  c->current_speed = tr_data->should_speed;
  c->last_speed = tr_data->last_speed;

  c->target_velocity = velocity_model[tr_data->should_speed];
  c->acceleration = acceleration;
}

void update_coordinates_after_reverse(coordinates *c) {
  location_reverse(&track, &c->loc, &c->loc);
  c->loc.offset += PICKUP_LENGTH;
}

void update_coordinates_after_time_passed(int clock_server, coordinates *c) {
  update_coordinates_helper(Time(clock_server), c);
}
