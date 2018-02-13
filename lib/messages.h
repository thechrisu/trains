/**
 * @file
 * @brief Message formats.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include "tstdlib.h"

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
  MESSAGE_PRINTF,
  REPLY_GETC
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
  int type;
  union {
    int32_t reply_time_ticks;
    int32_t message_delay_ticks;
    int32_t message_delay_until_ticks;
    k3_params reply_k3_params;
    char putc;
    char getc;
    printf_params printf;
  } msg;
} message;

#define EMPTY_MESSAGE (message *)0

#endif /* MESSAGE_H */
