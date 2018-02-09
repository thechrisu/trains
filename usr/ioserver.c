#include "ioserver.h"

#define TERMINAL_TX_BUF_SZ 20000
#define TERMINAL_RX_BUF_SZ 20000
#define TRAIN_TX_BUF_SZ 2000
#define TRAIN_RX_BUF_SZ 100

void generic_tx_server(uint16_t buf_sz, int channel) {
  int sender_tid;
  message received;
  char buf[buf_sz];
  char_buffer tx_buf;
  char_buffer_init(&tx_buf, buf, buf_sz);
  bool can_put = true;
  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    switch (received.type) {
      case MESSAGE_NOTIFIER:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        if (!char_buffer_is_empty(&tx_buf)) {
          can_put = false;
          Assert(rawcanputc(channel));
          Assert(rawputc(channel, char_buffer_get(&tx_buf) == 0));
        } else {
          can_put = true;
        }
        break;
      case MESSAGE_PUTC:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        if (can_put) {
          can_put = false;
          Assert(rawcanputc(channel));
          Assert(rawputc(channel, char_buffer_get(&tx_buf) == 0));
        } else {
          char_buffer_put(&tx_buf, received.msg.putc);
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
      case MESSAGE_NOTIFIER:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        char c;
        Assert(rawgetc(channel, &c));
        Assert(!char_buffer_is_full(&rx_buf));
        rawgetc(channel, &c); // Maybe assert this later
        char_buffer_put(&rx_buf, c);
        if (!int32_t_buffer_is_empty(&wait_buf)) {
          reply.type = REPLY_GETC;
          reply.msg.getc = char_buffer_get(&rx_buf);
          Assert(Reply(int32_t_buffer_get(&wait_buf), &reply, sizeof(reply)) >= 0);
        }
        break;
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
  Assert(Create(MyPriority() + 1, &train_tx_notifier) >= 0);
  generic_tx_server(TRAIN_TX_BUF_SZ, TRAIN);
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
  Assert(Create(MyPriority() + 1, &terminal_tx_notifier) >= 0);
  generic_tx_server(TERMINAL_TX_BUF_SZ, TERMINAL);
}

void terminal_rx_server() {
  Assert(RegisterAs("TerminalRxServer") == 0);
  Assert(WhoIs("TerminalRxServer") == MyTid());
  Assert(Create(MyPriority() + 1, &terminal_rx_notifier) >= 0);
  generic_rx_server(TERMINAL_RX_BUF_SZ, TERMINAL);
}

void spawn_ioservers() {
  Assert(Create(MyPriority() + 1, &train_tx_server) >= 0);
  Assert(Create(MyPriority() + 1, &train_rx_server) >= 0);
  Assert(Create(MyPriority() + 1, &terminal_tx_server) >= 0);
  Assert(Create(MyPriority() + 1, &terminal_rx_server) >= 0);
}
