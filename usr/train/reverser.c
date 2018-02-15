#include "reverser.h"

void reverser() {
  int sender_tid, train_tx_tid, clock_server_tid, track_state_controller_tid;
  char train_to_reverse;
  message received, get_train_msg, train_data_msg;

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  switch (received.type) {
    case MESSAGE_REVERSE:

      train_to_reverse = received.msg.reverser_params.train_to_reverse;
      train_tx_tid = received.msg.reverser_params.train_tx_server_tid;
      clock_server_tid = received.msg.reverser_params.clock_server_tid;
      track_state_controller_tid = received.msg.reverser_params.track_state_controller_tid;

      Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);

      get_train_msg.type = MESSAGE_GETTRAIN;
      get_train_msg.msg.train = train_to_reverse;

      Assert(Send(track_state_controller_tid, &get_train_msg, sizeof(get_train_msg),
                  &train_data_msg, sizeof(train_data_msg))
             > 0);

#if DEBUG_REVERSAL
      logprintf("Stopping train %d (speed %d) for reversal\n\r",
                train_to_reverse, train_data_msg.msg.tr_data.should_speed);
#endif /* DEBUG_REVERSAL */

      char initial_reverse_cmd[2];
      initial_reverse_cmd[0] = 0;
      initial_reverse_cmd[1] = train_to_reverse;
      PutBytes(train_tx_tid, initial_reverse_cmd, 2);

      Delay(clock_server_tid, train_data_msg.msg.tr_data.should_speed * 33); // 3 seconds

#if DEBUG_REVERSAL
      logprintf("Reversing train %d\n\r", train_to_reverse);
#endif /* DEBUG_REVERSAL */

      char send_reverse_cmd[2];
      send_reverse_cmd[0] = REVERSE_SPEED;
      send_reverse_cmd[1] = train_to_reverse;
      PutBytes(train_tx_tid, send_reverse_cmd, 2);

#if DEBUG_REVERSAL
      logprintf("Re-accelerating train %d after reversal to %d\n\r", train_to_reverse,
                train_data_msg.msg.tr_data.should_speed);
#endif /* DEBUG_REVERSAL */

      char final_speed_cmd[2];
      final_speed_cmd[0] = train_data_msg.msg.tr_data.should_speed;
      final_speed_cmd[1] = train_to_reverse;
      PutBytes(train_tx_tid, final_speed_cmd, 2);

      train_data_msg.type = MESSAGE_TRAINREVERSED;
      train_data_msg.msg.tr_data.direction = !train_data_msg.msg.tr_data.direction;
      Assert(Send(track_state_controller_tid, &train_data_msg, sizeof(train_data_msg),
                  EMPTY_MESSAGE, 0) >= 0);
      break;
    default:
      logprintf("Reverser got bad message of type %d\n\r", received.type);
      Assert(0);
      break;
    }
}
