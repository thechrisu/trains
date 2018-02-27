/**
 * @file
 * @brief Server for user commands.
 */
#ifndef COMMAND_DISPATCHER
#define COMMAND_DISPATCHER

#include "a0codes.h"
#include "codes.h"
#include "messages.h"
#include "track_node.h"
#include "user_command.h"
#include "train/commands.h"
#include "train/reverser.h"
#include "train/switcher.h"
#include "train/calibration/stopping_distance_calibrator.h"
#include "train/calibration/velocity_calibrator.h"

/**
 * Interprets messages sent from the train FirstUserTask and relays them
 * onto the train/the right direction, so that the FirstUserTask can unblock.
 *
 * !!! Needs to be created after the I/O servers.
 */
void command_dispatcher_server();

#endif /* COMMAND_DISPATCHER */
