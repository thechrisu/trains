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
};

typedef struct {
  int type;
  union msg {
    int reply_time_ticks;
    int message_delay_ticks;
    int message_delay_until_ticks;
  }
} message;

#endif /* MESSAGE_H */
