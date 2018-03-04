/**
 * @file
 * @brief Tests for the router
 */
#ifndef TEST_ROUTER_H
#define TEST_ROUTER_H

#include "clock.h"
#include "codes.h"
#include "messages.h"
#include "nameserver.h"
#include "../nameserver/ns_tid.h"
#include "router.h"
#include "track_state_controller.h"
#include "track_data.h"
#include "track_node.h"

void test_router_basic();
void test_router_sensor_pair();
void test_router_errors();

#endif /* TEST_ROUTER_H */
