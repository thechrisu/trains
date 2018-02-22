#include "switcher.h"

void switcher() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
  Assert(sender_tid == MyParentTid());
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
  Assert(received.type == MESSAGE_SWITCH);

  int clock_server_tid = received.msg.switch_params.clock_server_tid;
  int tx_server_tid = received.msg.switch_params.tx_server_tid;
  int track_state_controller_tid = WhoIs("TrackStateController");

  int turnout_num = received.msg.switch_params.turnout_num;
  bool curved = received.msg.switch_params.curved;
  switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, turnout_num, curved);
}
