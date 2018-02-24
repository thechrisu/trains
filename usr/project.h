/**
 * @file
 * @brief Trains project user tasks
 */
#ifndef PROJECT_H
#define PROJECT_H

#include "char_buffer.h"
#include "clock.h"
#include "codes.h"
#include "command_dispatcher.h"
#include "idle.h"
#include "ioserver.h"
#include "nameserver.h"
#include "messages.h"
#include "myio.h"
#include "terminal.h"
#include "track_data.h"
#include "tstdlib.h"
#include "tstring.h"
#include "user_command.h"
#include "train/sensor_secretary.h"
#include "train/turnout_resetter.h"
#include "train/track_state_controller.h"
#include "views/clock_view.h"
#include "views/sensor_view.h"
#include "views/turnout_view.h"

void project_first_user_task();

#endif /* PROJECT_H */