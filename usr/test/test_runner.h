/**
 * @file
 * @brief Function us to dynamically call different main functions.
 */
#ifndef TEST_RUNNER
#define TEST_RUNNER

#include "terminal.h"

#include "../k1.h"
#include "../k2.h"
#include "../k3.h"
#include "../project.h"
#include "./messaging/test_messaging.h"
#include "nameserver/test_nameserver_wrapper_errors.h"
#include "nameserver/test_nameserver_too_many.h"
#include "nameserver/test_nameserver_happypath.h"
#include "awaitevent/test_timer_interrupt.h"
#include "awaitevent/test_tx_terminal.h"
#include "awaitevent/test_rx_terminal.h"
#include "conductor/test_conductor.h"
#include "conductor/test_coordinate_courier.h"
#include "./iio/test_getcputc_errors.h"
#include "./iio/test_getcputc_happypath.h"
#include "./iio/test_printf.h"
#include "./test_undefined_handler.h"
#include "./test_message_benchmark.h"
#include "./test_mypriority.h"
#include "./model/test_model.h"
#include "clock/test_clock.h"
#include "destroy/test_destroy.h"
#include "prediction/test_stopping_location_prediction.h"
#include "prediction/test_collision_prediction.h"
#include "prediction/test_next_switch_prediction.h"
#include "router/test_router.h"
#include "./servers/test_track_reservation_server.h"
#include "./servers/test_train_coordinates_server.h"
#include "sleeper/test_sleeper.h"
#include "myio.h"
#include "tstdlib.h"

void test_runner();

#endif /* TEST_RUNNER */
