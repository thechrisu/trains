#include "reverser.h"

void reverser() {
  int sender_tid, train_tx_tid, clock_server_tid, track_state_controller_tid;
  char train_to_reverse;
  message received, get_train_msg, train_data_msg;

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  switch (received.type) {
    case MESSAGE_REVERSE:
      train_to_reverse = received.msg.reverser_params.train_to_reverse;
      Assert(train_to_reverse <= 81);
      train_tx_tid = received.msg.reverser_params.train_tx_server_tid;
      clock_server_tid = received.msg.reverser_params.clock_server_tid;
      track_state_controller_tid = received.msg.reverser_params.track_state_controller_tid;

      Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);

      get_train_msg.type = MESSAGE_GETTRAIN;
      get_train_msg.msg.tr_data.train = train_to_reverse;

      Assert(Send(track_state_controller_tid, &get_train_msg, sizeof(get_train_msg),
                  &train_data_msg, sizeof(train_data_msg))
             > 0);

      int should_speed = train_data_msg.msg.tr_data.should_speed;

#if DEBUG_REVERSAL
      logprintf("Stopping train %d (speed %d) for reversal\n\r", train_to_reverse, should_speed);
#endif /* DEBUG_REVERSAL */

      set_train_speed(train_tx_tid, track_state_controller_tid, train_to_reverse, 0);

      Delay(clock_server_tid, should_speed * 33); // 3 seconds

#if DEBUG_REVERSAL
      logprintf("Reversing train %d\n\r", train_to_reverse);
#endif /* DEBUG_REVERSAL */

      reverse_train(train_tx_tid, track_state_controller_tid, train_to_reverse);

#if DEBUG_REVERSAL
      logprintf("Re-accelerating train %d after reversal to %d\n\r", train_to_reverse, should_speed);
#endif /* DEBUG_REVERSAL */

      set_train_speed(train_tx_tid, track_state_controller_tid, train_to_reverse, should_speed);
      break;
    default:
      logprintf("Reverser got bad message of type %d\n\r", received.type);
      Assert(0);
      break;
    }
}
