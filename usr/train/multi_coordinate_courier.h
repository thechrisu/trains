/**
 * @file
 * @brief Multitrain Coordinate courier: Multi-Train conductor's bitch.
 */
#ifndef MULTI_TRAIN_COORDINATE_COURIER_H
#define MULTI_TRAIN_COORDINATE_COURIER_H

#include "codes.h"
#include "commands.h"
#include "messages.h"
#include "track_data.h"
#include "tstdlib.h"
#include "train_util.h"

/**
 * Checks whether the given coordinates trigger notifications,
 * and gives us the first notification that would trigger.
 *
 * @param c                        The current position of the train.
 * @param last                     The position of the train, the previous time
 *                                 we called this function.
 * @param locations_to_observe     Array of possible locations that may trigger
 *                                 a notification.
 * @param is_location_set          Array of same size as locations_to_observe
 *                                 to tell us which indices of
 *                                 locations_to_observe are actual notifications.
 * @param n                        Output argument: The notification to trigger.
 * @return Whether or not a notification was triggered.
 */
bool multi_coordinates_to_notification(coordinates *c, coordinates *last,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE],
        location_notification *n);

/**
 * Function to add multiple requests for a notification to the array holding
 * the notification promises.
 *
 * @param notifications            Array of requests.
 * @param n_requests               Number of requests.
 * @param locations_to_observe     Potentially nonempty array of existing
 *                                 observations. We'll add to this array.
 * @param is_location_set          Tells us the indices of existing notifications.
 *                                 We'll update this array.
 */
int multi_add_notification_requests(
        location_notification notifications[MAX_LOCATIONS_TO_OBSERVE],
        int n_requests,
        location_notification locations_to_observe[MAX_LOCATIONS_TO_OBSERVE],
        bool is_location_set[MAX_LOCATIONS_TO_OBSERVE]);

/**
 * Coordinate courier task
 */
void multi_coordinate_courier();

/**
 * Creates a coordinate courier task and initializes it to serve a particular
 * train. To be called from the train coordinate courier's conductor task.
 */
int create_multi_courier(train_group *group);

#endif /* MULTI_TRAIN_COORDINATE_COURIER_H */

