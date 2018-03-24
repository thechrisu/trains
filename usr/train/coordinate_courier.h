#ifndef TRAIN_COORDINATE_COURIER_H
#define TRAIN_COORDINATE_COURIER_H

#include "codes.h"
#include "messages.h"

#define GOT_LOST               -1
#define NOTIFICATION_TO_SWITCH  0
#define LOCATION_REACHED        1

typedef struct {
  int reason;
  union {
    char switch_to_switch[2]; // 0: switch number, 1: state to switch to.
    unsigned int sensor_triggered; // sensor offset triggered.
  };
} notification_reason;

typedef struct {
  track_node *track_node;
  notification_reason reason;
} location_notification;

void coordinate_courier();

#endif /* TRAIN_COORDINATE_COURIER_H */

