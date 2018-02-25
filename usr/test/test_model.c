#include "test_model.h"

void test_constant_velocity_model() {
  ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(ns_tid > 0);
  int track_state_controller_tid = Create(MyPriority() + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);
  message receive;
  get_constant_velocity_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 15; i++) {
    Assert(receive.msg.train_speeds[i] == 0);
  }

  update_constant_velocity_model(track_state_controller_tid, 1, 14, sensor_offset('B', 7), sensor_offset('A', 10), 90);

  get_constant_velocity_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 14; i++) {
    Assert(receive.msg.train_speeds[i] == 0);
  }
  // 289 mm, 90 ticks, divide by 10 for EWMA calculation
  Assert(receive.msg.train_speeds[14] == 289 * 10000 / 90 / 10);

  Assert(Kill(track_state_controller_tid) == 0);

  bwprintf("Success.\n\r");
}
