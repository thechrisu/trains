/**
 * @file
 * @brief Message formats.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include "model_data.h"
#include "track_node.h"
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
  MESSAGE_TURNOUTSWITCHED,
  MESSAGE_CALIB_SD,
  MESSAGE_CALIB_V,
  MESSAGE_GETCONSTANTSPEEDMODEL,
  REPLY_GETCONSTANTSPEEDMODEL,
  MESSAGE_UPDATECONSTANTSPEEDMODEL,
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
  int turnout_num;
  bool curved;
} message_switch_params;

typedef struct {
  unsigned int turnout_num;
  turnout_state state;
} message_turnout_switched_params;

typedef struct {
  int train;
  int speed;
} message_calib_sd_params;

typedef struct {
  int train;
} message_calib_v_params;

typedef struct {
  int train;
  int speed;
  unsigned int start;
  unsigned int end;
  int ticks;
} message_ucsm_params;

typedef struct {
  int type;
  union {
    int32_t reply_time_ticks;
    int32_t message_delay_ticks;
    int32_t message_delay_until_ticks;
    k3_params reply_k3_params;
    char putc;
    char getc;
    char train;
    printf_params printf;
    user_command cmd;
    int nameserver_response;
    message_reverse_params reverser_params;
    message_switch_params switch_params;
    train_data tr_data;
    int16_t sensors[10];
    int32_t train_speeds[15];
    message_turnout_switched_params turnout_switched_params;
    turnout_state turnout_states[NUM_TURNOUTS];
    message_calib_sd_params calib_sd_params;
    message_calib_v_params calib_v_params;
    message_ucsm_params ucsm;
  } msg;
} message;

#define EMPTY_MESSAGE (message *)0

#endif /* MESSAGE_H */
