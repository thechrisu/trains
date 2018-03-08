#ifndef ROUTER_H
#define ROUTER_H

#include "codes.h"
#include "messages.h"
#include "myio.h"
#include "../lib/search_node_queue.h"
#include "track_data.h"
#include "track_node.h"
#include "tstdlib.h"

#define MAX_ROUTE_LENGTH SEARCH_NODE_QUEUE_SIZE

/**
 * Task that plans routes between different points on the track.
 */
void router();

/**
 * Reserves a route between two locations and returns it to the caller.
 *
 * @param   train The train to reserve the route for.
 * @param   start The start of the route.
 * @param   end   The end of the route.
 * @param   res   An array of reservations into which the resulting route is placed.
 * @returns 0 on success.
 * @returns -1 if there is already a reserved route for the train.
 */
int get_route(int train, int speed, location *start, location *end, reservation route[MAX_ROUTE_LENGTH]);

int get_route_next(int train, int speed, location *start, location *end, reservation route[MAX_ROUTE_LENGTH]);

/**
 * Cancels the route reserved by the given train.
 *
 * @param   train The train to cancel the route of.
 * @returns 0 on success.
 *          -1 if the given train does not have a reserved route.
 */
int cancel_route(int train);

/**
 * @param   route An array of reservations, terminated with a reservation with `train`
 *                equal to zero.
 * @returns The length of the route.
 */
int route_length(reservation route[MAX_ROUTE_LENGTH]);

#endif /* ROUTER_H */
