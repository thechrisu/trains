#include "test_model.h"

void assert_valid_velocity(int speed, int velocity) {
  if (speed == 0) {
    Assert(velocity == 0);
  } else {
    Assert(velocity > 0);
    Assert(velocity > speed * 4782 - 8000);
    Assert(velocity < speed * 4782 + 8000);
  }
}

void test_constant_velocity_model() {
  ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(ns_tid > 0);

  int track_state_controller_tid = Create(MyPriority() + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);

  message receive;
  get_constant_velocity_model(track_state_controller_tid, 1, &receive);

  uint32_t first_train_speeds[15];
  first_train_speeds[0] = 0;

  for (int i = 0; i < 15; i++) {
    unsigned int this_speed = receive.msg.train_speeds[i];
    assert_valid_velocity(i, receive.msg.train_speeds[i]);
    first_train_speeds[i] = this_speed;
  }

  update_constant_velocity_model(track_state_controller_tid, 1, 14, sensor_offset('C', 15), sensor_offset('D', 12), 60);

  get_constant_velocity_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 14; i++) {
    Assert(receive.msg.train_speeds[i] == first_train_speeds[i]);
  }
  assert_valid_velocity(14, receive.msg.train_speeds[14]);
  first_train_speeds[14] = receive.msg.train_speeds[14];

  update_constant_velocity_model(track_state_controller_tid, 1, 14, sensor_offset('C', 15), sensor_offset('C', 15), 0);
  for (int i = 0; i <= 14; i++) {
    Assert(receive.msg.train_speeds[i] == first_train_speeds[i]);
  }

  Assert(Kill(track_state_controller_tid) == 0);

  bwprintf("Success.\n\r");
}
