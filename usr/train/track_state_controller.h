/**
 * @file
 * @brief Track State Controller.
 */
#ifndef TRACK_STATE_CONTROLLER
#define TRACK_STATE_CONTROLLER

#include "codes.h"
#include "model.h"
#include "messages.h"
#include "train_coordinates_server.h"
#include "../track/track_data.h"
#include "../track/track_node.h"

/**
 * Source of truth for the train track.
 */
void track_state_controller();

#endif /* TRACK_STATE_CONTROLLER */
