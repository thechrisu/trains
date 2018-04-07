/**
 * @file
 * @brief Trains project user tasks
 */
#ifndef PROJECT_H
#define PROJECT_H

#include "anti_freezer.h"
#include "char_buffer.h"
#include "clock.h"
#include "codes.h"
#include "command_dispatcher.h"
#include "event_server.h"
#include "idle.h"
#include "ioserver.h"
#include "nameserver.h"
#include "messages.h"
#include "myio.h"
#include "parameters.h"
#include "terminal.h"
#include "track_data.h"
#include "tstdlib.h"
#include "tstring.h"
#include "user_command.h"
#include "train/router.h"
#include "train/train_model_courier.h"
#include "train/sensors/sensor_secretary.h"
#include "train/sensors/sensor_interpreter.h"
#include "train/speed_resetter.h"
#include "train/turnout_resetter.h"
#include "train/track_reservation_server.h"
#include "train/track_state_controller.h"
#include "views/clock_view.h"
#include "views/reservation_view.h"
#include "views/sensor_view.h"
#include "views/train_location_view.h"
#include "views/turnout_view.h"

void project_first_user_task();

#endif /* PROJECT_H */
