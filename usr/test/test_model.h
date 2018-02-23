/**
 * @file
 * @brief Test functions for the train model.
 */
#ifndef TEST_MODEL_H
#define TEST_MODEL_H

#include "codes.h"
#include "messages.h"
#include "nameserver.h"
#include "./nameserver/ns_tid.h"
#include "../train/commands.h"
#include "../train/track_state_controller.h"

void test_constant_velocity_model();

#endif /* TEST_MODEL_H */
