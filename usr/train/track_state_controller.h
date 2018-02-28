/**
 * @file
 * @brief Track State Controller.
 */
#ifndef TRACK_STATE_CONTROLLER
#define TRACK_STATE_CONTROLLER

#include "codes.h"
#include "model.h"
#include "messages.h"
#include "../track/track_data.h"
#include "../track/track_node.h"

extern track_state track;

/**
 * Source of truth for the train track.
 */
void track_state_controller();

#endif /* TRACK_STATE_CONTROLLER */
