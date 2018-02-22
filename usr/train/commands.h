#ifndef TRAIN_COMMANDS_H
#define TRAIN_COMMANDS_H

#include "codes.h"
#include "messages.h"

void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed);

#endif /* TRAIN_COMMANDS_H */
