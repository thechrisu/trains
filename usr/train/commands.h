#ifndef TRAIN_COMMANDS_H
#define TRAIN_COMMANDS_H

#include "a0codes.h"
#include "codes.h"
#include "messages.h"

void set_train_speed(int train_tx_server_tid, int track_state_controller_tid, int train, int speed);

void reverse_train(int train_tx_server_tid, int track_state_controller_tid, int train, bool current_direction);

void switch_turnout(int clock_server_tid, int train_tx_server_tid, int track_state_controller_tid, int turnout_num, bool curved);

#endif /* TRAIN_COMMANDS_H */
