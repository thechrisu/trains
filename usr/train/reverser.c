#include "reverser.h"

void reverser() {
  int sender_tid, train_tx_tid, clock_server_tid, track_state_controller_tid;
  char train_to_reverse;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  switch (received.type) {
    case MESSAGE_REVERSE:
      train_to_reverse = received.msg.reverser_params.train_to_reverse;
      Assert(train_to_reverse <= 81);
      train_tx_tid = received.msg.reverser_params.train_tx_server_tid;
      clock_server_tid = received.msg.reverser_params.clock_server_tid;
      track_state_controller_tid = received.msg.reverser_params.track_state_controller_tid;

      Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);

      stop_and_reverse_train(clock_server_tid, train_tx_tid, track_state_controller_tid, train_to_reverse);
      break;
    default:
      logprintf("Reverser got bad message of type %d\n\r", received.type);
      Assert(0);
      break;
    }
}
