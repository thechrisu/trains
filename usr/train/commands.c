#include "commands.h"

void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed) {
  set_train_speed_and_headlights(train_tx_server_tid, track_state_controller_tid, train, speed, true);
}

void set_train_speed_and_headlights(int train_tx_server_tid, int track_state_controller_tid, int train, int speed, bool headlights) {
  if (speed < 0 || speed >= 15) {
    logprintf("Setting %d to %d (%d)\n\r", train, speed, Time(WhoIs("ClockServer")));
  }
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
#if DEBUG_SWITCHING
  logprintf("Sw %d to %s\n\r", turnout_num, curved ? "C" : "S");
#endif /* DEBUG_SWITCHING */

  message send;
  send.type = MESSAGE_TURNOUTSWITCHED;
  send.msg.turnout_switched_params.turnout_num = turnout_num;
  send.msg.turnout_switched_params.state = curved ? TURNOUT_CURVED : TURNOUT_STRAIGHT;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);

  if (turnout_num >= 153 && turnout_num <= 156 && curved) {
    int paired_turnouts[] = { 154, 153, 156, 155 };
    int paired_turnout = paired_turnouts[turnout_num - 153];
    buf[0] = (char)0x21;
    buf[1] = (char)paired_turnout;
    Assert(PutBytes(train_tx_server_tid, buf, 2) == 0);
#if DEBUG_SWITCHING
    logprintf("Sw paired %d to straight\n\r", paired_turnout);
#endif /* DEBUG_SWITCHING */

    send.msg.turnout_switched_params.turnout_num = paired_turnout;
    send.msg.turnout_switched_params.state = TURNOUT_STRAIGHT;
    Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
  }

  Delay(clock_server_tid, 15);
  Putc(train_tx_server_tid, TRAIN, (char)0x20);
}

void switcher_turnout(int clock_server_tid, int train_tx_server_tid,
                      int turnout_num, bool curved) {
  message send;
  send.type = MESSAGE_SWITCH;
  send.msg.switch_params.clock_server_tid = clock_server_tid;
  send.msg.switch_params.tx_server_tid = train_tx_server_tid;
  send.msg.switch_params.turnout_num = turnout_num;
  send.msg.switch_params.curved = curved;
  int switcher_tid = Create(MyPriority() + 2, &switcher);
  Assert(Send(switcher_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void get_train(int track_state_controller_tid, int train, train_data *tr_data) {
  message send, reply;
  send.type = MESSAGE_GETTRAIN;
  send.msg.tr_data.train = train;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
  Assert(reply.type == REPLY_GETTRAIN);
  tmemcpy(tr_data, &reply.msg.tr_data, sizeof(*tr_data));
}

void get_sensors(int track_state_controller_tid, message *reply) {
  message send;
  send.type = MESSAGE_GETSENSORS;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), reply, sizeof(*reply)) == sizeof(*reply));
  Assert(reply->type == REPLY_GETSENSORS);
}

void get_turnouts(int track_state_controller_tid, turnout_state turnout_states[NUM_TURNOUTS]) {
  message send, reply;
  send.type = MESSAGE_GETTURNOUTS;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
  Assert(reply.type == REPLY_GETTURNOUTS);
  tmemcpy(turnout_states, &reply.msg.turnout_states, NUM_TURNOUTS * sizeof(turnout_state));
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

void get_stopping_distance_model(int track_state_controller_tid, int train, message *reply) {
  message send;
  send.type = MESSAGE_GETSTOPPINGDISTANCEMODEL;
  send.msg.train = train;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), reply, sizeof(*reply)) == sizeof(*reply));
  Assert(reply->type == REPLY_GETSTOPPINGDISTANCEMODEL);
}

void update_stopping_distance_model(int track_state_controller_tid, int train, int speed, uint32_t distance) {
  message send;
  send.type = MESSAGE_UPDATESTOPPINGDISTANCEMODEL;
  send.msg.usdm.train = train;
  send.msg.usdm.speed = speed;
  send.msg.usdm.value = distance;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void get_stopping_time_model(int track_state_controller_tid, int train, message *reply) {
  message send;
  send.type = MESSAGE_GETSTOPPINGTIMEMODEL;
  send.msg.train = train;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), reply, sizeof(*reply)) == sizeof(*reply));
  Assert(reply->type == REPLY_GETSTOPPINGTIMEMODEL);
}

void update_stopping_time_model(int track_state_controller_tid, int train, int speed, uint32_t time) {
  message send;
  send.type = MESSAGE_UPDATESTOPPINGTIMEMODEL;
  send.msg.ustm.train = train;
  send.msg.ustm.speed = speed;
  send.msg.ustm.value = time;
  Assert(Send(track_state_controller_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void get_last_sensor_hit(int sensor_interpreter_tid, int train, reply_get_last_sensor_hit *last_sensor) {
  message send, reply;
  send.type = MESSAGE_GET_LAST_SENSOR_HIT;
  send.msg.train = train;
  Assert(Send(sensor_interpreter_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));
  Assert(reply.type == REPLY_GET_LAST_SENSOR_HIT);
  tmemcpy(last_sensor, &reply.msg.last_sensor, sizeof(*last_sensor));
}

void get_coordinates(int train_coordinates_server, int train, coordinates *c) {
  message send, reply;
  send.type = MESSAGE_GET_COORDINATES;
  send.msg.train = train;
  Assert(Send(train_coordinates_server,
              &send, sizeof(send),
              &reply, sizeof(reply)) == sizeof(reply));
  Assert(reply.type == REPLY_GET_COORDINATES);
  tmemcpy(c, &reply.msg.coords, sizeof(*c));
}

int send_to_track_reservation_server(int track_reservation_server,
                                     int train,
                                     track_node *start,
                                     track_node *end,
                                     enum message_type send_type,
                                     enum message_type expected_reply_type) {
  message send, reply;

  send.type = send_type;
  send.msg.reservation_request.train = train;
  send.msg.reservation_request.nodes[0] = start;
  send.msg.reservation_request.nodes[1] = end;

  Assert(Send(track_reservation_server,
              &send, sizeof(send),
              &reply, sizeof(reply)) == sizeof(reply));

  Assert(reply.type == expected_reply_type);
  return reply.msg.reservation_response;
}

int reservation_make(int track_reservation_server, int train,
                     track_node *start, track_node *end) {
  return send_to_track_reservation_server(track_reservation_server,
                                          train, start, end,
                                          MESSAGE_RESERVATION_MAKE,
                                          REPLY_RESERVATION_MAKE);
}

int reservation_drop(int track_reservation_server, int train,
                     track_node *start, track_node *end) {
  return send_to_track_reservation_server(track_reservation_server,
                                          train, start, end,
                                          MESSAGE_RESERVATION_DROP,
                                          REPLY_RESERVATION_DROP);
}

void reservation_drop_all(int track_reservation_server, int train) {
  message send;

  send.type = MESSAGE_RESERVATION_DROP_ALL;
  send.msg.reservation_request.train = train;

  Assert(Send(track_reservation_server,
              &send, sizeof(send),
              EMPTY_MESSAGE, 0) == 0);
}

void reservation_get_all(int track_reservation_server, int train,
                         message_reservation_get_all_response *reservations) {
  message send, reply;

  send.type = MESSAGE_RESERVATION_GET_ALL;
  send.msg.reservation_request.train = train;

  Assert(Send(track_reservation_server,
              &send, sizeof(send),
              &reply, sizeof(reply)) == sizeof(reply));

  Assert(reply.type == REPLY_RESERVATION_GET_ALL);

  tmemcpy(reservations, &reply.msg.all_reservations, sizeof(*reservations));
}

unsigned int get_train_destination(int command_dispatcher, int train) {
  message send, reply;

  send.type = MESSAGE_GET_DESTINATION;
  send.msg.train = train;

  Assert(Send(command_dispatcher,
              &send, sizeof(send),
              &reply, sizeof(reply)) == sizeof(reply));

  Assert(reply.type == REPLY_GET_DESTINATION);

  return reply.msg.destination;
}
