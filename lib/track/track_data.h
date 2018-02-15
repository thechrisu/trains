/**
 * @file
 * @brief Helper functions for track/train state
 */
#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include "track_node.h"
#include "tstdlib.h"

void init_track(track_state *global_track);

unsigned int turnout_num_to_map_offset(unsigned int turnout);

unsigned int map_offset_to_turnout(unsigned int offset);

bool is_valid_turnout_num(unsigned int turnout);

#endif /* TRACK_DATA_H */
