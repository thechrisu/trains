#include "commands.h"

void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed) {
  set_train_speed_and_headlights(train_tx_server_tid, track_state_controller_tid, train, speed, false);
}

void set_train_speed_and_headlights(int train_tx_server_tid, int track_state_controller_tid, int train, int speed, bool headlights) {
  Assert(train >= 1 && train <= 80);
  Assert(speed >= 0 && speed <= 14);

  char to_send_bytes[2];
  to_send_bytes[0] = speed + (headlights ? 0x10 : 0);
  to_send_bytes[1] = train;
  Assert(PutBytes(train_tx_server_tid, to_send_bytes, 2) == 0);

  message send;
  send.type = MESSAGE_TRAINSETSPEED;
  send.msg.tr_data.train = train;
  send.msg.tr_data.should_speed = speed;
  send.msg.tr_data.headlights = headlights;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void reverse_train(int train_tx_server_tid, int track_state_controller_tid, int train) {
  Assert(train > 0 && train <= 80);

  char send_reverse_cmd[2];
  send_reverse_cmd[0] = REVERSE_SPEED;
  send_reverse_cmd[1] = train;
  Assert(PutBytes(train_tx_server_tid, send_reverse_cmd, 2) == 0);

  message send, reply;
  send.type = MESSAGE_GETTRAIN;
  send.msg.tr_data.train = train;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

  send.type = MESSAGE_TRAINREVERSED;
  send.msg.tr_data.train = train;
  send.msg.tr_data.direction = !reply.msg.tr_data.direction;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void stop_and_reverse_helper(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int train, int previous_speed, int speed) {
#if DEBUG_REVERSAL
  logprintf("Stopping train %d (speed %d) for reversal\n\r", train, previous_speed);
#endif /* DEBUG_REVERSAL */

  set_train_speed(train_tx_server_tid, track_state_controller_tid, train, 0);

  Delay(clock_server_tid, previous_speed * 33); // 3 seconds

#if DEBUG_REVERSAL
  logprintf("Reversing train %d\n\r", train);
#endif /* DEBUG_REVERSAL */

  reverse_train(train_tx_server_tid, track_state_controller_tid, train);
  Delay(clock_server_tid, 8);

#if DEBUG_REVERSAL
  logprintf("Re-accelerating train %d after reversal to %d\n\r", train, speed);
#endif /* DEBUG_REVERSAL */

  set_train_speed(train_tx_server_tid, track_state_controller_tid, train, speed);
}

void stop_and_reverse_train_to_speed(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int train, int speed) {
  message received, reply;

  received.type = MESSAGE_GETTRAIN;
  received.msg.tr_data.train = train;

  Assert(Send(track_state_controller_tid, &received, sizeof(received), &reply, sizeof(reply)) > 0);

  int should_speed = reply.msg.tr_data.should_speed;
  stop_and_reverse_helper(clock_server_tid, train_tx_server_tid, track_state_controller_tid, train, should_speed, speed);
}

void stop_and_reverse_train(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int train) {
  message received, reply;

  received.type = MESSAGE_GETTRAIN;
  received.msg.tr_data.train = train;

  Assert(Send(track_state_controller_tid, &received, sizeof(received), &reply, sizeof(reply)) > 0);

  int should_speed = reply.msg.tr_data.should_speed;
  stop_and_reverse_helper(clock_server_tid, train_tx_server_tid, track_state_controller_tid, train, should_speed, should_speed);
}

void switch_turnout(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int turnout_num, bool curved) {
  Assert(is_valid_turnout_num(turnout_num));

  char buf[2];
  buf[0] = (char)(curved ? 0x22 : 0x21);
  buf[1] = (char)turnout_num;
  Assert(PutBytes(train_tx_server_tid, buf, 2) == 0);

  message send;
  send.type = MESSAGE_TURNOUTSWITCHED;
  send.msg.turnout_switched_params.turnout_num = turnout_num;
  send.msg.turnout_switched_params.state = curved ? TURNOUT_CURVED : TURNOUT_STRAIGHT;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);

  Delay(clock_server_tid, 15);
  Putc(train_tx_server_tid, TRAIN, (char)0x20);
}

void get_sensors(int track_state_controller_tid, message *reply) {
  message send;
  send.type = MESSAGE_GETSENSORS;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), reply, sizeof(*reply)) == sizeof(*reply));
  Assert(reply->type == REPLY_GETSENSORS);
}

void get_constant_velocity_model(int track_state_controller_tid, int train, message *reply) {
  message send;
  send.type = MESSAGE_GETCONSTANTSPEEDMODEL;
  send.msg.train = train;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), reply, sizeof(*reply)) == sizeof(*reply));
  Assert(reply->type == REPLY_GETCONSTANTSPEEDMODEL);
}

void update_constant_velocity_model(int track_state_controller_tid, int train, int speed, unsigned int start, unsigned int end, uint32_t ticks) {
  message send;
  send.type = MESSAGE_UPDATECONSTANTSPEEDMODEL;
  send.msg.ucsm.train = train;
  send.msg.ucsm.speed = speed;
  send.msg.ucsm.start = start;
  send.msg.ucsm.end = end;
  send.msg.ucsm.ticks = ticks;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}
