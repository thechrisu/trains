#include "test_model.h"

void test_constant_velocity_model() {
  ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(ns_tid > 0);
  int track_state_controller_tid = Create(MyPriority() + 1, &track_state_controller);
  Assert(track_state_controller_tid > 0);
  message send, receive;

  send.type = MESSAGE_GETCONSTANTSPEEDMODEL;
  send.msg.train = 1;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &receive, sizeof(receive)) >= 0);

  Assert(receive.type == REPLY_GETCONSTANTSPEEDMODEL);
  for (int i = 0; i < 15; i++) {
    Assert(receive.msg.train_speeds[i] == 0);
  }


  send.type = MESSAGE_UPDATECONSTANTSPEEDMODEL;
  send.msg.ucsm.train = 1;
  send.msg.ucsm.speed = 14;
  send.msg.ucsm.velocity = 1337;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &receive, sizeof(receive)) >= 0);


  send.type = MESSAGE_GETCONSTANTSPEEDMODEL;
  send.msg.train = 1;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &receive, sizeof(receive)) >= 0);

  Assert(receive.type == REPLY_GETCONSTANTSPEEDMODEL);
  for (int i = 0; i < 14; i++) {
    Assert(receive.msg.train_speeds[i] == 0);
  }
  Assert(receive.msg.train_speeds[14] == 1337);

  Assert(Kill(track_state_controller_tid) == 0);

  bwprintf("Success.\n\r");
}
