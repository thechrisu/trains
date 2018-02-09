#include "test_getcputc_happypath.h"

void test_getcputc_mirror() {
  int ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(Create(MyPriority() - 3, &idle_task) > 0);
  int sender_tid = Create(MyPriority() + 1, &terminal_tx_server);
  int receiver_tid = Create(MyPriority() + 1, &terminal_rx_server);
  Assert(sender_tid >= 0);
  Assert(receiver_tid >= 0);
  while (true) {
    char c = Getc(receiver_tid, TERMINAL);
    Assert(c >= 0);
    if (c == 0) break;
    Putc(sender_tid, TERMINAL, c);
  }
  Kill(sender_tid);
  Kill(receiver_tid);
  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}

void test_get_sensors() {
  int ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(Create(MyPriority() - 3, &idle_task) > 0);
  int sender_tid = Create(MyPriority() + 1, &train_tx_server);
  int receiver_tid = Create(MyPriority() + 1, &train_rx_server);
  Assert(sender_tid >= 0);
  Assert(receiver_tid >= 0);
  Putc(sender_tid, TRAIN, CMD_ALL_SENSORS);
  for (int i = 0; i < 10; i++) {
    int c = Getc(receiver_tid, TRAIN);
    Assert(c >= 0);
    bwprintf("Sensor %d: %c", i, c);
  }
  Kill(WhoIs("TrainTxNotifier"));
  Kill(WhoIs("TrainRxNotifier"));
  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}

void test_go_stop() {
  int ns_tid = Create(MyPriority() + 1, &nameserver_main);
  Assert(Create(MyPriority() - 3, &idle_task) > 0);
  int sender_tid = Create(MyPriority() + 1, &train_tx_server);
  int receiver_tid = Create(MyPriority() + 1, &terminal_rx_server);
  Assert(sender_tid >= 0);
  Assert(receiver_tid >= 0);
  int go = 0;
  while (true) {
    int c = Getc(receiver_tid, TERMINAL);
    Assert(c >= 0);
    if (c == 'q') break;
    if (go) {
      Putc(sender_tid, TRAIN, CMD_GO);
    } else {
      Putc(sender_tid, TRAIN, CMD_STOP);
    }
    go = (go + 1) % 2;
  }
  Kill(WhoIs("TrainTxNotifier"));
  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}
