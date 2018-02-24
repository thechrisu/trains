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

  // B7 to A10
  update_constant_velocity_model(track_state_controller_tid, 1, 14, 39, 10, 1337);

  get_constant_velocity_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 14; i++) {
    Assert(receive.msg.train_speeds[i] == 0);
  }
  Assert(receive.msg.train_speeds[14] == 1337);

  Assert(Kill(track_state_controller_tid) == 0);

  bwprintf("Success.\n\r");
}