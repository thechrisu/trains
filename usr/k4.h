/**
 * @file
 * @brief K4 user tasks
 */
#ifndef K4_H
#define K4_H

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
#include "tstdlib.h"
#include "tstring.h"
#include "user_command.h"
#include "trains/switch_resetter.h"
#include "views/clock_view.h"

void k4_first_user_task();

#endif /* K4_H */
