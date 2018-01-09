#ifndef TRACK_DATA_H
#define TRACK_DATA_H

#include "track_node.h"
#include "../stdlib.h"

void init_track(struct track_state *global_track);

unsigned int turnout_num_to_map_offset(unsigned int turnout);

unsigned int map_offset_to_turnout(unsigned int offset);

#endif /* TRACK_DATA_H */
