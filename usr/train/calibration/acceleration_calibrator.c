#include "acceleration_calibrator.h"

enum accel_state {
  WAITING_CONSTANT_SPEED,
  HAS_NOT_YET_ACCELERATED,
  WAITING_BACK_TO_CONSTANT_SPEED,
  HAS_ACCELERATED,
};

#define NTRAINS_ARRAY 81

void dynamic_acceleration_calibrator() {
  int sensor_interpreter_tid = WhoIs("SensorInterpreter");
  int track_state_controller = WhoIs("TrackStateController");
  int clock_server = WhoIs("ClockServer");
  int trains[NTRAINS_ARRAY];
  int num_trains = 1;
  enum accel_state accel_states[NTRAINS_ARRAY];
  bool has_first_sensor[NTRAINS_ARRAY];
  message stopping_time_models[NTRAINS_ARRAY];
  long long loops  = 0;
  while (true) {
    Delay(clock_server, 2 * REFRESH_PERIOD);
    if (loops++ % 100 == 0) { // periodically update the stopping distance model.
      for (int i = 0; i < num_trains; i++) {
        get_stopping_time_model(track_state_controller,
                                trains[i],
                                (stopping_time_models + trains[i]));
      }
    }
    reply_get_last_sensor_hit start_sensor[NTRAINS_ARRAY];
    for (int i = 0; i < num_trains; i++) {
      trains[0] = t1train;
      int train = trains[i];
      reply_get_last_sensor_hit query;
      get_last_sensor_hit(sensor_interpreter_tid, trains[i], &query);
      train_data tr_data;
      get_train(track_state_controller, train, &tr_data);
      enum train_mode last_mode = get_train_mode(&tr_data,
                                stopping_time_models[train].msg.train_times);
      switch (accel_states[train]) { // TODO if we have speed 0 inbetween, reset to WAITING_CONSTANT_SPEED
        case WAITING_CONSTANT_SPEED:
          if (!sensor_hit_is_equal(&query, start_sensor + train)) {
            if (last_mode != CONSTANT_VELOCITY) {
              has_first_sensor[train] = true;
            } else if (last_mode == CONSTANT_VELOCITY && has_first_sensor[train]) {
              accel_states[train] = HAS_NOT_YET_ACCELERATED;
            }
          }
          break;
        case HAS_NOT_YET_ACCELERATED:
          if (last_mode != CONSTANT_VELOCITY) {
            accel_states[train] = WAITING_BACK_TO_CONSTANT_SPEED;
          }
          break;
        case WAITING_BACK_TO_CONSTANT_SPEED:
          if (last_mode == CONSTANT_VELOCITY) {
            accel_states[train] = HAS_ACCELERATED;
          }
          break;
        case HAS_ACCELERATED:
          if (!sensor_hit_is_equal(&query, start_sensor + train)) {
            int tick_diff = query.ticks - start_sensor[train].ticks;
            uint32_t mm_diff = (query.sensor - start_sensor[train].sensor);
            logprintf("When accelerating, tick diff between sensors %d and %d is %d. mm diff is %d\n\r",
                    start_sensor[train].sensor, query.sensor, tick_diff, mm_diff);
            accel_states[train] = HAS_NOT_YET_ACCELERATED;
          }
          break;
        default:
          Assert(0); // Something is completely wrong.
          break;
      }
      if (has_first_sensor[train]) {
        start_sensor[train].sensor = query.sensor;
        start_sensor[train].ticks = query.ticks;
      }
    }
  }
}
