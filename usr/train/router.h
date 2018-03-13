#ifndef ROUTER_H
#define ROUTER_H

#include "codes.h"
#include "commands.h"
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
 * Finds a route between two locations.
 *
 * @param   train The train to reserve the route for.
 * @param   start The start of the route.
 * @param   end   The end of the route.
 * @param   res   An array into which the resulting route is placed.
 * @returns 0 on success.
 *          -1 if no route could be found.
 */
int get_route(location *start, location *end, track_node *route[MAX_ROUTE_LENGTH]);

/**
 * Reserves a route between the sensor after the start location and the end location.
 *
 * @param   train The train to reserve the route for.
 * @param   start The location before the sensor at the start of the route.
 * @param   end   The end of the route.
 * @param   res   An array into which the resulting route is placed.
 * @returns 0 on success.
 *          -1 if no route could be found.
 */
int get_route_next(location *start, location *end, track_node *route[MAX_ROUTE_LENGTH]);

/**
 * @param   route A route.
 * @returns The number of nodes in the route.
 */
int route_node_count(track_node *route[MAX_ROUTE_LENGTH]);

#endif /* ROUTER_H */
