#include "acceleration_calibrator.h"

#define ABS(a) (a < 0 ? -a : a)

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
// Returns distance in 1/100mm OR -1 on error.
int distance_between_sensors_intermediate(unsigned long between[MAX_S_LIST],
                                          int n_between,
                                          unsigned long start,
                                          unsigned long end) {
  Assert(start <= 80 && end <= 80);
#if ACC_CALIB_DEBUG
  logprintf("Start: %c%d, End: %c%d\n\r",
            sensor_bank(start), sensor_index(start),
            sensor_bank(end), sensor_index(end));
#endif /* ACC_CALIB_DEBUG */
  if (n_between == 0) {
    return (int)distance_between_sensors(&track, start, end) * 100;
  } else {
#if ACC_CALIB_DEBUG
    logprintf("Start: %c%d, between[0]: %c%d\n\r",
              sensor_bank(start), sensor_index(start),
              sensor_bank(between[0]), sensor_index(between[0]));
#endif /* ACC_CALIB_DEBUG */
    int total = distance_between_sensors(&track, start, between[0]);
    for (int i = 0; i < n_between - 1; i++) {
#if ACC_CALIB_DEBUG
      logprintf("between[i]: %c%d, between[i + 1]: %c%d\n\r",
                sensor_bank(between[i]), sensor_index(between[i]),
                sensor_bank(between[i + 1]), sensor_index(between[i + 1]));
#endif /* ACC_CALIB_DEBUG */
      int d = distance_between_sensors(&track, between[i], between[i + 1]);
      if (d == -1) return -1;
      total += d;
    }
    return total * 100;
  }
}

int get_accel_ticks(reply_get_last_sensor_hit *start,
                    reply_get_last_sensor_hit *end,
                    unsigned long between[MAX_S_LIST],
                    int n_between,
                    int ticks_accel_sent, int v_0, int v_1) {
  if (v_1 == v_0) return 0;
  int d_d = (int)distance_between_sensors_intermediate(between, n_between,
                                               start->sensor, end->sensor);
  if (d_d == -1) return -1;
  int d_t = end->ticks - start->ticks;
  // TODO subtract overshoot time
  int t_w = ticks_accel_sent - start->ticks;
#if ACC_CALIB_DEBUG
  logprintf("d_d: %d, d_t: %d, t_w: %d\n\r", d_d, d_t, t_w);
#endif /* ACC_CALIB_DEBUG */
  // Only multiplying by 100.0 to get more granularity.
  logprintf("d_t * v_1: %d, t_w * v_1: %d, t_w * v_0: %d\n\r",
            d_t * v_1, t_w * v_1, t_w * v_0);
  if (d_t < 0 || t_w < 0) return -1;
  int numerator = S_TO_TICKS(d_d - TICKS_TO_S(d_t) * v_1 + TICKS_TO_S(t_w) * v_1 - TICKS_TO_S(t_w) * v_0);
  return ABS(2 * numerator / (v_0 - v_1));
}

/**
 * @param v_0               Start velocity (1/100 mm/s).
 * @param v_1               End velocity (1/1/00 mm/s).
 * @param t_a               Time to accelerate between v_0, v_1 (ticks).
 * @return acceleration     1/100 mm/(s^2)
 */
int get_acceleration(int v_0, int v_1, int t_a) {
  if (t_a == 0) return 0;
  return (v_1 - v_0) / TICKS_TO_S(t_a);
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
            || accel_states[train] == HAS_ACCELERATED)
                && has_first_sensor[train]
                && !sensor_hit_is_equal(&query, start_sensor + train)
                && !sensor_hit_is_equal(&query, last_sensor + train)
                && query.sensor <= 80) {
#if ACC_CALIB_DEBUG
        logprintf("Between: %c%d (Start: %c%d)\n\r", sensor_bank(query.sensor),
                  sensor_index(query.sensor),
                  sensor_bank(start_sensor[train].sensor),
                  sensor_index(start_sensor[train].sensor));
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
            n_between[train] = 0;
            if (query.sensor <= 80)
              memcpy(start_sensor + train, &query, sizeof(query));
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
            logprintf(
                "V: %d, End: %d, n_between: %d, acc: %d\n\r", v,
                end_velocities[train], n_between[train],
                query_snapshot.acceleration);
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
              int accel_ticks = get_accel_ticks(
                                  start_sensor + train,
                                  &query,
                                  between[train], n_between[train],
                                  ticks_when_accel[train],
                                  start_velocities[train],
                                  end_velocities[train]);
              if (accel_ticks == -1) {
                // TODO don't log
                goto cleanup; // I'm sorry.
              }
              int ticks_0_14 = 0.5 * get_acceleration(
                                            start_velocities[train],
                                            end_velocities[train],
                                            accel_ticks) * 3 * 3;
              logprintf(
                  "%s(%d -> %d)%s%c%d and %c%d is %d. (acc: %d), 3s: %d\n\r",
                  "When accelerating ", start_velocities[train],
                  end_velocities[train], " ticks to accelerate between ",
                  sensor_bank(start_sensor[train].sensor),
                  sensor_index(start_sensor[train].sensor),
                  sensor_bank(query.sensor), sensor_index(query.sensor),
                  accel_ticks,
                  get_acceleration(start_velocities[train],
                                   end_velocities[train], accel_ticks),
                  ticks_0_14);
            }
cleanup:
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
