#include "notifier.h"

void generic_notifier(enum event_id to_wait_on, int server_tid) {
  Assert(server_tid > 0);

  message msg;
  msg.type = MESSAGE_NOTIFIER;
  while (true) {
    Assert(AwaitEvent(to_wait_on) == 0);
    Assert(Send(server_tid, &msg, sizeof(msg), EMPTY_MESSAGE, 0) >= 0);
  }
  Assert("ACCIDENTALLY KILLED NOTIFIER\n\r" == "");
  Assert(0);
}

void train_tx_notifier() {
  Assert(RegisterAs("TrainTxNotifier") == 0);
  Assert(WhoIs("TrainTxNotifier") == MyTid());
  generic_notifier(TRAIN_TX_INTERRUPT, MyParentTid());
}

void train_rx_notifier() {
  Assert(RegisterAs("TrainRxNotifier") == 0);
  Assert(WhoIs("TrainRxNotifier") == MyTid());
  generic_notifier(TRAIN_RX_INTERRUPT, MyParentTid());
}

void terminal_tx_notifier() {
  Assert(RegisterAs("TerminalTxNotifier") == 0);
  Assert(WhoIs("TerminalTxNotifier") == MyTid());
  generic_notifier(TERMINAL_TX_INTERRUPT, MyParentTid());
}

void terminal_rx_notifier() { // TODO make this more general by passing in the names/interrupts via messsages (and registering this task as MyParentTid() + X)...
  Assert(RegisterAs("TerminalRxNotifier") == 0);
  Assert(WhoIs("TerminalRxNotifier") == MyTid());
  generic_notifier(TERMINAL_RX_INTERRUPT, MyParentTid());
}

/*
  Repeatedly waits for a clock event, then sends a message to the clock server.
  Created by the clock server, so sends its messages to its parent.
*/
void clock_notifier() {
  Assert(RegisterAs("ClockNotifier") == 0);
  Assert(WhoIs("ClockNotifier") == MyTid());
  generic_notifier(TIMER_INTERRUPT, MyParentTid());
}
