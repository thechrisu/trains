#ifndef TRAIN_COORD_COURIER_TEST_H
#define TRAIN_COORD_COURIER_TEST_H

#include "clock.h"
#include "codes.h"
#include "global_track_state.h"
#include "messages.h"
#include "nameserver.h"
#include "../nameserver/ns_tid.h"
#include "track_state_controller.h"
#include "track_data.h"
#include "track_node.h"

#include "coordinate_courier.h"

int coord_courier_test_setup();
void coord_courier_test_teardown(int courier_tid);


void test_coord_courier();

#endif /* TRAIN_COORD_COURIER_TEST_H */
