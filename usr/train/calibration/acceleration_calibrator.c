#include "acceleration_calibrator.h"

/**
 * WAITING_CONSTANT_SPEED
 * |
 * | - has hit a sensor
 * | - has acceleration == 0
 * V
 * HAS_NOT_YET_ACCELERATED
 * |
 * | - has acceleration != 0
 * V
 * WAITING_BACK_TO_CONSTANT_SPEED
 * |
 * | - has acceleration == 0
 * V
 * HAS_ACCELERATED
 *   - We are done when we have hit a sensor other than the start sensor
 */
enum accel_state {
  WAITING_CONSTANT_SPEED,
  HAS_NOT_YET_ACCELERATED,
  WAITING_BACK_TO_CONSTANT_SPEED,
  HAS_ACCELERATED,
};

#define NTRAINS_ARRAY 81
#define FIND_LIMIT_ACC 20
#define MAX_S_LIST 10

// NOTE: Between sensors includes the last sensor
int distance_between_sensors_intermediate(unsigned long between[MAX_S_LIST],
                                          int n_between,
                                          unsigned long start,
                                          unsigned long end) {
  Assert(start <= 80 && end <= 80);
  if (n_between == 0) {
    return distance_between_sensors(&track, start, end);
  } else {
    int total = distance_between_sensors(&track, start, between[0]);
    for (int i = 0; i < n_between - 1; i++) {
      total += distance_between_sensors(&track, between[i], between[i + 1]);
    }
    return total;
  }
}

int get_accel_ticks(reply_get_last_sensor_hit *start,
                    reply_get_last_sensor_hit *end,
                    unsigned long between[MAX_S_LIST],
                    int n_between,
                    int ticks_accel_sent, int v_0, int v_1) {
  if (v_1 == v_0) return 0;
  int d_d = distance_between_sensors_intermediate(between, n_between,
                                           start->sensor, end->sensor);
  int d_t = end->ticks - start->ticks;
  // TODO subtract overshoot time
  int t_w = ticks_accel_sent - start->ticks;
  int numerator = d_d - d_t * v_1 + t_w * v_1 - t_w * v_0;
  return 2 * numerator / (v_0 - v_1);
}

int get_acceleration(int v_0, int v_1, int t_a) {
  if (t_a == 0) return 0;
  return (v_1 - v_0) / t_a;
}

void dynamic_acceleration_calibrator() {
  int sensor_interpreter_tid = WhoIs("SensorInterpreter");
  Assert(sensor_interpreter_tid > 0);
  int track_state_controller = WhoIs("TrackStateController");
  Assert(track_state_controller > 0);
  int coordinates_server = WhoIs("TrainCoordinatesServer");
  Assert(coordinates_server > 0);
  int clock_server = WhoIs("ClockServer");
  Assert(clock_server > 0);

  int trains[NTRAINS_ARRAY];
  int num_trains = 1;
  enum accel_state accel_states[NTRAINS_ARRAY];
  unsigned long between[NTRAINS_ARRAY][MAX_S_LIST];
  int n_between[NTRAINS_ARRAY];
  for (int i = 0; i < NTRAINS_ARRAY; i++) {
    accel_states[i] = WAITING_CONSTANT_SPEED;
    n_between[i] = 0;
  }
  bool has_first_sensor[NTRAINS_ARRAY];
  reply_get_last_sensor_hit start_sensor[NTRAINS_ARRAY];
  reply_get_last_sensor_hit last_sensor[NTRAINS_ARRAY];
  int start_velocities[NTRAINS_ARRAY], end_velocities[NTRAINS_ARRAY];
  int ticks_when_accel[NTRAINS_ARRAY];
  bool is_forward_at_start[NTRAINS_ARRAY];
  while (true) {
    // TODO update this for t2trains
    trains[0] = t1train;
    Delay(clock_server, REFRESH_PERIOD);
    for (int i = 0; i < num_trains; i++) {
      int train = trains[i];
      reply_get_last_sensor_hit query;
      get_last_sensor_hit(sensor_interpreter_tid, train, &query);
      coordinates query_snapshot;
      get_coordinates(coordinates_server, train, &query_snapshot);
      if ((accel_states[train] == WAITING_BACK_TO_CONSTANT_SPEED
            || HAS_ACCELERATED) && has_first_sensor[train]
                && !sensor_hit_is_equal(&query, start_sensor + train)
                && !sensor_hit_is_equal(&query, last_sensor + train)
                && query.sensor <= 80) {
#if ACC_CALIB_DEBUG
        logprintf("Between: %c%d\n\r", sensor_bank(query.sensor),
                  sensor_index(query.sensor));
#endif /* ACC_CALIB_DEBUG */
        if (query.sensor == sensor_pair(&track, start_sensor[train].sensor)) {
          accel_states[train] = WAITING_CONSTANT_SPEED;
        } else {
          between[train][n_between[train]] = query.sensor;
          n_between[train] += 1;
        }
      }
      if (query_snapshot.target_velocity == 0
          && accel_states[train] != WAITING_CONSTANT_SPEED
          && accel_states[train] != HAS_NOT_YET_ACCELERATED) {
        n_between[train] = 0;
        accel_states[train] = WAITING_CONSTANT_SPEED;
      }
      switch (accel_states[train]) {
        case WAITING_CONSTANT_SPEED:
          if (!sensor_hit_is_equal(&query, start_sensor + train)
                  && query.sensor <= 80) {
            has_first_sensor[train] = true;
          }
          if (query_snapshot.acceleration == 0
              && has_first_sensor[train]) {
#if ACC_CALIB_DEBUG
            logprintf("TO CONST SPEED\n\r");
#endif /* ACC_CALIB_DEBUG */
            accel_states[train] = HAS_NOT_YET_ACCELERATED;
            train_data tr_data;
            get_train(track_state_controller, train, &tr_data);
            is_forward_at_start[train] = tr_data.direction;
            start_velocities[train] = query_snapshot.target_velocity;
            n_between[train] = 0;
            if (query.sensor <= 80)
              memcpy(start_sensor + train, &query, sizeof(query));
          }
          break;
        case HAS_NOT_YET_ACCELERATED:
          if (query_snapshot.acceleration != 0) {
            accel_states[train] = WAITING_BACK_TO_CONSTANT_SPEED;
#if ACC_CALIB_DEBUG
            logprintf("TO ACCELERATE\n\r");
#endif /* ACC_CALIB_DEBUG */
            train_data tr_data;
            get_train(track_state_controller, train, &tr_data);
            ticks_when_accel[train] = tr_data.time_speed_last_changed;
            end_velocities[train] = query_snapshot.target_velocity;
          }
          break;
        case WAITING_BACK_TO_CONSTANT_SPEED:
          if (query_snapshot.acceleration == 0) {
#if ACC_CALIB_DEBUG
            logprintf("TO END OF ACCELERATION\n\r");
#endif /* ACC_CALIB_DEBUG */
            accel_states[train] = HAS_ACCELERATED;
          }
          break;
        case HAS_ACCELERATED:
          if (!sensor_hit_is_equal(&query, last_sensor + train)
                  && query.sensor <= 80) {
            train_data tr_data;
            get_train(track_state_controller, train, &tr_data);
            int v = query_snapshot.target_velocity;
#if ACC_CALIB_DEBUG
            logprintf("V: %d, End: %d, n_between: %d, acc: %d\n\r", v, end_velocities[train], n_between[train], query_snapshot.acceleration);
#endif /* ACC_CALIB_DEBUG */
            if (tr_data.direction == is_forward_at_start[train]
                && end_velocities[train] != 0
                && query_snapshot.acceleration == 0
                && v == end_velocities[train]
                && !sensor_hit_is_equal(&query, start_sensor)
                && n_between[train] < MAX_S_LIST) {
#if ACC_CALIB_DEBUG
              logprintf("Start: %c%d, End: %c%d\n\r",
                        sensor_bank(start_sensor[train].sensor),
                        sensor_index(start_sensor[train].sensor),
                        sensor_bank(query.sensor),
                        sensor_index(query.sensor));
#endif /* ACC_CALIB_DEBUG */
              int accel_ticks = get_accel_ticks(start_sensor + train,
                                  &query,
                                  between[train], n_between[train],
                                  ticks_when_accel[train],
                                  start_velocities[train],
                                  end_velocities[train]);
              logprintf(
                  "%s(%d -> %d)%s%c%d and %c%d is %d.\n\r",
                  "When accelerating ", start_velocities[train],
                  end_velocities[train], " ticks to accelerate ",
                sensor_bank(start_sensor[train].sensor), sensor_index(start_sensor[train].sensor),
                sensor_bank(query.sensor), sensor_index(query.sensor), accel_ticks);
            }
            accel_states[train] = WAITING_CONSTANT_SPEED;
            n_between[train] = 0;
          }
          break;
        default:
          Assert(0); // Something is completely wrong.
          break;
      }
      if (!sensor_hit_is_equal(&query,
                               last_sensor + train) && query.sensor <= 80) {
        memcpy(last_sensor + train, &query, sizeof(query));
      }
    }
  }
}
