#include "reverser.h"

void reverse() {
  int sender_tid, train_tx_tid, clock_server_tid;
  char train_to_reverse;
  message received;
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    switch (received.type) {
      case MESSAGE_REVERSE:
        train_to_reverse = received.msg.reverser_params.train_to_reverse;
        train_tx_tid = received.msg.reverser_params.train_tx_server_tid;
        clock_server_tid = received.msg.reverser_params.clock_server_tid;
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        char initial_reverse_cmd[2];
        initial_reverse_cmd[0] = 0;
        initial_reverse_cmd[1] = train_to_reverse;
        PutBytes(train_tx_tid, initial_reverse_cmd, 2);
        // TODO base length of delay off the train's current speed.
        Delay(clock_server_tid, 300); // 3 seconds
        char send_reverse_cmd[2];
        send_reverse_cmd[0] = 15;
        send_reverse_cmd[1] = train_to_reverse;
        PutBytes(train_tx_tid, initial_reverse_cmd, 2);
        // TODO reaccelerate
        // Putc(train_tx_tid, TRAIN, train_to_reverse)
        break;
      default:
        logprintf("Reverser got bad message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
}
