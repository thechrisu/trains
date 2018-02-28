#include "test_model.h"

void test_stopping_distance_model() {
  ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(ns_tid > 0);
  int track_state_controller_tid = Create(MyPriority() + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);
  message receive;
  get_stopping_distance_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 15; i++) {
    Assert(receive.msg.train_distances[i] == 0);
  }

  update_stopping_distance_model(track_state_controller_tid, 1, 14, 133700);

  get_stopping_distance_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 14; i++) {
    Assert(receive.msg.train_distances[i] == 0);
  }
  Assert(receive.msg.train_distances[14] == 133700);

  Assert(Kill(track_state_controller_tid) == 0);

  bwprintf("Success.\n\r");
}