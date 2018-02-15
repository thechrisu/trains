#include "switch_resetter.h"

static int tx_server_tid;
static int command_dispatcher_tid;

void switch_turnout(int switch_num, bool curved) {
  message send;
  send.type = MESSAGE_USER;
  send.msg.cmd.type = USER_CMD_SW;
  send.msg.cmd.data[0] = (char)switch_num;
  send.msg.cmd.data[1] = (char)(curved ? 'C' : 'S');
  Assert(Send(command_dispatcher_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void disable_solenoid() {
  Putc(tx_server_tid, TRAIN, (char)0x20);
}

void switch_resetter() {
  int clock_server_tid = WhoIs("ClockServer");
  tx_server_tid = WhoIs("TrainTxServer");
  command_dispatcher_tid = WhoIs("CommandDispatcher");

  for (int i = 1; i <= 17; i += 2) {
    switch_turnout(i, true);
    switch_turnout(i + 1, true);
    Delay(clock_server_tid, 15);
    disable_solenoid();
  }

  switch_turnout(153, true);
  switch_turnout(154, false);
  Delay(clock_server_tid, 15);
  disable_solenoid();

  switch_turnout(155, true);
  switch_turnout(156, false);
  Delay(clock_server_tid, 15);
  disable_solenoid();
}
