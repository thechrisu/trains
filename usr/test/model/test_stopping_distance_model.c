#include "test_model.h"

int sd_polynomial_val(int speed);

int sd_polynomial_val(int speed) {
  return 100 * (0.80616977 * speed * speed * speed
    - 5.37389122 * speed * speed + 16.86623686 * speed - 9.08379739);
}

void assert_valid_stopping_distance(int speed, int distance) {
  if (speed == 0) {
    Assert(distance == 0);
  } else {
    Assert(distance > 0);
    Assert(distance > sd_polynomial_val(speed) * 0.9 - 1000);
    Assert(distance < sd_polynomial_val(speed) * 1.1 + 1000);
  }
}

void assert_in_bounds(int val, int exp, int bounds);

void assert_in_bounds(int val, int exp, int bounds) {
  Assert(val < exp + bounds);
  Assert(val > exp - bounds);
}

void test_stopping_distance_model() {
  ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(ns_tid > 0);
  int track_state_controller_tid = Create(MyPriority() + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);
  message receive;

  get_stopping_distance_model(track_state_controller_tid, 1, &receive);
  for (int i = 0; i < 14; i++) {
    assert_valid_stopping_distance(i, receive.msg.train_distances[i]);
  }

  update_stopping_distance_model(track_state_controller_tid, 1, 14, 133700);
  get_stopping_distance_model(track_state_controller_tid, 1, &receive);
  bwprintf("%d\n\r", receive.msg.train_distances[14]);
  assert_in_bounds(receive.msg.train_distances[14], 133700, 20000);

  Assert(Kill(track_state_controller_tid) == 0);

  bwprintf("Success.\n\r");
}
