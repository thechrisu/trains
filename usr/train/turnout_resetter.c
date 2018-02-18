#include "turnout_resetter.h"

static int clock_server_tid;
static int track_state_controller_tid;
static int tx_server_tid;

void switch_turnout(int turnout_num, bool curved) {
  message send;

  char buf[2];
  buf[0] = (char)(curved ? 0x22 : 0x21);
  buf[1] = (char)turnout_num;
  PutBytes(tx_server_tid, buf, 2);

  send.type = MESSAGE_TURNOUTSWITCHED;
  send.msg.turnout_switched_params.turnout_num = turnout_num;
  send.msg.turnout_switched_params.state = curved ? TURNOUT_CURVED : TURNOUT_STRAIGHT;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) >= 0);

  Delay(clock_server_tid, 15);
  Putc(tx_server_tid, TRAIN, (char)0x20);
}

void turnout_resetter() {
  clock_server_tid = WhoIs("ClockServer");
  tx_server_tid = WhoIs("TrainTxServer");
  track_state_controller_tid = WhoIs("TrackStateController");

  for (int i = 1; i <= 18; i += 1) {
    switch_turnout(i, true);
  }
  switch_turnout(153, true);
  switch_turnout(154, false);
  switch_turnout(155, true);
  switch_turnout(156, false);
}
