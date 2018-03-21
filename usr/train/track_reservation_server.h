/**
 * @file
 * @brief Server that allows train conductors to reserve pieces of track
 */
#ifndef TRACK_RESERVATION_SERVER_H
#define TRACK_RESERVATION_SERVER_H

#include "codes.h"
#include "global_track_state.h"
#include "messages.h"
#include "track_data.h"
#include "track_node.h"

#define RESERVATION_ENCODE(start, end) ((start) * TRACK_MAX + (end))
#define RESERVATION_START(code)        ((code) / TRACK_MAX)
#define RESERVATION_END(code)          ((code) % TRACK_MAX)

void track_reservation_server();

#endif /* TRACK_RESERVATION_SERVER_H */
