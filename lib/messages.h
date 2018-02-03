#ifndef MESSAGE_H
#define MESSAGE_H

#include "clock.h"

#define EMPTY_MESSAGE (void *)0

enum message_type {
  MESSAGE_CLOCK_NOTIFIER,
  MESSAGE_TIME,
  REPLY_TIME,
  MESSAGE_DELAY,
  MESSAGE_DELAY_UNTIL,
  REPLY_CLOCK_SERVER_ERROR,
};

typedef struct {
  int type;
  union {
    int32_t reply_time_ticks;
    int32_t message_delay_ticks;
    int32_t message_delay_until_ticks;
  } msg;
} message;

#endif /* MESSAGE_H */
