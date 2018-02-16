/**
 * @file
 * @brief Message formats.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include "track_data.h"
#include "tstdlib.h"
#include "user_command.h"

#define PRINTF_MESSAGE_BUFFER_SIZE 256

enum message_type {
  MESSAGE_NOTIFIER,
  MESSAGE_TIME,
  REPLY_TIME,
  MESSAGE_DELAY,
  MESSAGE_DELAY_UNTIL,
  REPLY_CLOCK_SERVER_OK,
  REPLY_CLOCK_SERVER_ERROR,
  MESSAGE_K3_GET_PARAMS,
  REPLY_K3_GET_PARAMS,
  MESSAGE_K3_DONE,
  MESSAGE_PUTC,
  MESSAGE_GETC,
  REPLY_GETC,
  MESSAGE_PRINTF,
  REPLY_NAMESERVER,
  MESSAGE_USER,
  MESSAGE_REVERSE,
  MESSAGE_SWITCH,
  MESSAGE_GETTRAIN,
  REPLY_GETTRAIN,
  MESSAGE_GETSENSORS,
  REPLY_GETSENSORS,
  MESSAGE_SENSORSRECEIVED,
  MESSAGE_GETTURNOUTS,
  REPLY_GETTURNOUTS,
  MESSAGE_TRAINSETSPEED,
  MESSAGE_TRAINREVERSED,
  MESSAGE_TURNOUTSWITCHED
};

typedef struct {
  int32_t delay;
  int32_t delay_count;
} k3_params;

typedef struct {
  char *buf;
  uint32_t size;
} printf_params;

typedef struct {
  int train_tx_server_tid;
  int clock_server_tid;
  int track_state_controller_tid;
  char train_to_reverse;
} message_reverse_params;

typedef struct {
  int clock_server_tid;
  int tx_server_tid;
  int switch_num;
  bool curved;
} message_switch_params;

typedef struct {
  unsigned int turnout_num;
  turnout_state state;
} message_turnout_switched_params;

typedef struct {
  int type;
  union {
    int32_t reply_time_ticks;
    int32_t message_delay_ticks;
    int32_t message_delay_until_ticks;
    k3_params reply_k3_params;
    char putc;
    char getc;
    printf_params printf;
    user_command cmd;
    int nameserver_response;
    message_reverse_params reverser_params;
    message_switch_params switch_params;
    train_data tr_data;
    char train;
    int16_t sensors[10];
    message_turnout_switched_params turnout_switched_params;
    turnout_state turnout_states[NUM_TURNOUTS];
  } msg;
} message;

#define EMPTY_MESSAGE (message *)0

#endif /* MESSAGE_H */
