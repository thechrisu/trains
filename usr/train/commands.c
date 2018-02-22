#include "commands.h"

void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed) {
  char to_send_bytes[2];
  to_send_bytes[0] = speed;
  to_send_bytes[1] = train;
  Assert(PutBytes(train_tx_server_tid, to_send_bytes, 2) == 0);

  message train_data_msg;
  train_data_msg.type = MESSAGE_TRAINSETSPEED;
  train_data_msg.msg.train = train;
  train_data_msg.msg.tr_data.should_speed = speed;
  Assert(Send(track_state_controller_tid, &train_data_msg, sizeof(train_data_msg), EMPTY_MESSAGE, 0) >= 0);
}
