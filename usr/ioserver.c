#include "ioserver.h"

void generic_tx_server(uint16_t buf_sz, int channel, int notifier_tid) {
  int sender_tid;
  message received;
  char buf[buf_sz];
  char_buffer tx_buf;
  char_buffer_init(&tx_buf, buf, buf_sz);
  bool can_put = false;
  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    switch (received.type) {
      case MESSAGE_NOTIFIER:
        if (!char_buffer_is_empty(&tx_buf)) {
          can_put = false;
          Assert(rawcanputc(channel));
          Assert(rawputc(channel, char_buffer_get(&tx_buf)) == 0);
          Assert(Reply(notifier_tid, EMPTY_MESSAGE, 0) >= 0);
        } else {
          can_put = true;
        }
        break;
      case MESSAGE_PUTC:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        Assert(!char_buffer_is_full(&tx_buf));
        char_buffer_put(&tx_buf, received.msg.putc);
        if (can_put) {
          can_put = false;
          Assert(rawcanputc(channel));
          Assert(!char_buffer_is_empty(&tx_buf));
          Assert(rawputc(channel, char_buffer_get(&tx_buf)) == 0);
          Assert(Reply(notifier_tid, EMPTY_MESSAGE, 0) >= 0);
        }
        break;
      case MESSAGE_PRINTF:
        for (uint32_t i = 0; i < received.msg.printf.size; i += 1) {
          if (char_buffer_is_full(&tx_buf)) {
            logprintf("Transmit buffer of tx server (channel %d), notifier %d full\n\r",
                      channel, notifier_tid);
            Assert(!char_buffer_is_full(&tx_buf));
          }
          char_buffer_put(&tx_buf, received.msg.printf.buf[i]);
        }

        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);

        if (received.msg.printf.size > 0 && can_put) {
          can_put = false;
          Assert(rawcanputc(channel));
          Assert(!char_buffer_is_empty(&tx_buf));
          Assert(rawputc(channel, char_buffer_get(&tx_buf)) == 0);
          Assert(Reply(notifier_tid, EMPTY_MESSAGE, 0) >= 0);
        }
        break;
      default:
        Assert(0);
    }
  }
  Assert(0);
}

void generic_rx_server(uint16_t buf_sz, int channel) {
  int sender_tid;
  message received, reply;
  char buf[buf_sz];
  char_buffer rx_buf;
  char_buffer_init(&rx_buf, buf, buf_sz);
  int32_t wait[MAX_TASKS];
  int32_t_buffer wait_buf;
  int32_t_buffer_init(&wait_buf, wait, MAX_TASKS);

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    switch (received.type) {
      case MESSAGE_NOTIFIER: {
        char c;
        Assert(rawcangetc(channel));
        int err = rawgetc(channel, &c);
        if (err) {
          logprintf("OE: %x, BE: %x, PE: %x, FE: %x\n\r", err & OE_MASK, err & BE_MASK, err & PE_MASK, err & FE_MASK);
        }
        Assert(!char_buffer_is_full(&rx_buf));
        char_buffer_put(&rx_buf, c);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        if (!int32_t_buffer_is_empty(&wait_buf)) {
          reply.type = REPLY_GETC;
          reply.msg.getc = char_buffer_get(&rx_buf);
          Assert(Reply(int32_t_buffer_get(&wait_buf), &reply, sizeof(reply)) >= 0);
        }
        break;
      }
      case MESSAGE_GETC:
        if (!char_buffer_is_empty(&rx_buf)) {
          reply.type = REPLY_GETC;
          reply.msg.getc = char_buffer_get(&rx_buf);
          Assert(Reply(sender_tid, &reply, sizeof(reply)) >= 0);
        } else {
          int32_t_buffer_put(&wait_buf, sender_tid);
        }
        break;
      default:
        Assert(0);
    }
  }
  Assert(0);
}

void train_tx_server() {
  Assert(RegisterAs("TrainTxServer") == 0);
  Assert(WhoIs("TrainTxServer") == MyTid());
  int notifier_tid = Create(MyPriority() + 1, &train_tx_notifier);
  Assert(notifier_tid >= 0);
  generic_tx_server(TRAIN_TX_BUF_SZ, TRAIN, notifier_tid);
}

void train_rx_server() {
  Assert(RegisterAs("TrainRxServer") == 0);
  Assert(WhoIs("TrainRxServer") == MyTid());
  Assert(Create(MyPriority() + 1, &train_rx_notifier) >= 0);
  generic_rx_server(TRAIN_RX_BUF_SZ, TRAIN);
}

void terminal_tx_server() {
  Assert(RegisterAs("TerminalTxServer") == 0);
  Assert(WhoIs("TerminalTxServer") == MyTid());
  int notifier_tid = Create(MyPriority() + 1, &terminal_tx_notifier);
  Assert(notifier_tid >= 0);
  generic_tx_server(TERMINAL_TX_BUF_SZ, TERMINAL, notifier_tid);
}

void terminal_rx_server() {
  Assert(RegisterAs("TerminalRxServer") == 0);
  Assert(WhoIs("TerminalRxServer") == MyTid());
  Assert(Create(MyPriority() + 1, &terminal_rx_notifier) >= 0);
  generic_rx_server(TERMINAL_RX_BUF_SZ, TERMINAL);
}

void spawn_ioservers() {
  Assert(Create(MyPriority() + 10, &train_tx_server) >= 0);
  Assert(Create(MyPriority() + 10, &train_rx_server) >= 0);
  Assert(Create(MyPriority() + 10, &terminal_tx_server) >= 0);
  Assert(Create(MyPriority() + 10, &terminal_rx_server) >= 0);
}

void kill_ioservers() {
  Assert(Kill(WhoIs("TrainTxNotifier")) == 0);
  Assert(Kill(WhoIs("TrainTxServer")) == 0);
  Assert(Kill(WhoIs("TrainRxNotifier")) == 0);
  Assert(Kill(WhoIs("TrainRxServer")) == 0);
  Assert(Kill(WhoIs("TerminalTxNotifier")) == 0);
  Assert(Kill(WhoIs("TerminalTxServer")) == 0);
  Assert(Kill(WhoIs("TerminalRxNotifier")) == 0);
  Assert(Kill(WhoIs("TerminalRxServer")) == 0);
}
