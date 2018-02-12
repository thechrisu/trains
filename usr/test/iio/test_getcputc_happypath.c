#include "test_getcputc_happypath.h"

void test_getcputc_mirror() {
  EnableCaches(true);
  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 3, &clock_server);
  /*
    Save idle task's tid and call Kill on that, instead of using the nameserver to look up
    the idle task, because the idle task may never have a chance to run:
    https://lists.nongnu.org/archive/html/qemu-devel/2017-09/msg02370.html
  */
  int idle_tid = Create(MyPriority() - 3, &idle_task);
  Assert(idle_tid > 0);
  int sender_tid = Create(MyPriority() + 1, &terminal_tx_server);
  int receiver_tid = Create(MyPriority() + 1, &terminal_rx_server);
  Assert(sender_tid >= 0);
  Assert(receiver_tid >= 0);
  while (true) {
    int c = Getc(receiver_tid, TERMINAL);
    Assert(c >= 0);
    if (c == 'q') break;
    Assert(Putc(sender_tid, TERMINAL, c) == 0);
  }
  Assert(Kill(sender_tid) == 0);
  Assert(Kill(receiver_tid) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(WhoIs("TerminalTxNotifier")) == 0);
  Assert(Kill(WhoIs("TerminalRxNotifier")) == 0);
  Assert(Kill(clock_server_tid) == 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}

void test_get_sensors() {
  EnableCaches(true);
  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 3, &clock_server);
  Assert(Create(MyPriority() - 3, &idle_task) > 0);
  int sender_tid = Create(MyPriority() + 1, &train_tx_server);
  int receiver_tid = Create(MyPriority() + 1, &train_rx_server);
  Assert(sender_tid >= 0);
  Assert(receiver_tid >= 0);
  Assert(Putc(sender_tid, TRAIN, CMD_ALL_SENSORS) == 0);
  for (int i = 0; i < 10; i++) {
    int c = Getc(receiver_tid, TRAIN);
    Assert(c >= 0);
    bwprintf("Sensor %d: %x\n\r", i, c);
  }
  Assert(Kill(WhoIs("TrainTxNotifier")) == 0);
  Assert(Kill(WhoIs("TrainRxNotifier")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(clock_server_tid) == 0);
  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}

void test_go_stop() {
  EnableCaches(true);
  ns_tid = Create(MyPriority() + 3, &nameserver_main);
  int clock_server_tid = Create(MyPriority() + 3, &clock_server);
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
      Assert(Putc(sender_tid, TRAIN, CMD_GO) == 0);
    } else {
      Assert(Putc(sender_tid, TRAIN, CMD_STOP) == 0);
    }
    go = (go + 1) % 2;
  }
  Assert(Kill(WhoIs("TerminalRxNotifier")) == 0);
  Assert(Kill(WhoIs("TrainTxNotifier")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(clock_server_tid) == 0);
  int idle_tid = WhoIs("Idle");
  Assert(idle_tid > 0);
  Assert(Kill(idle_tid) == 0);
  Assert(Kill(ns_tid) == 0);
}
