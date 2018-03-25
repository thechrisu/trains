#ifndef TRAIN_COORDINATE_COURIER_H
#define TRAIN_COORDINATE_COURIER_H

#include "codes.h"
#include "commands.h"
#include "messages.h"
#include "track_data.h"
#include "tstdlib.h"

bool coordinates_to_notification(coordinates *c, coordinates *last,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE],
        location_notification *n);
void coordinate_courier();
int create_courier(int train);

#endif /* TRAIN_COORDINATE_COURIER_H */

