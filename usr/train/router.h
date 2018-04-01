#ifndef ROUTER_H
#define ROUTER_H

#include "codes.h"
#include "commands.h"
#include "messages.h"
#include "myio.h"
#include "../lib/search_node_queue.h"
#include "track_data.h"
#include "track_node.h"
#include "train_util.h"
#include "tstdlib.h"

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
 * @param   route A route.
 * @returns The number of nodes in the route.
 */
int route_node_count(track_node *route[MAX_ROUTE_LENGTH]);

/**
 * @param   route A route.
 * @returns The length of the route in 1/100 mm.
 */
int route_length(track_node *route[MAX_ROUTE_LENGTH]);

/**
 * Prepare to travel between the given locations. Tell the caller to exit
 * if it isn't possible to reroute.
 * @param start                       Current location.
 * @param end                         Goal of our route.
 * @param route                       Place to put the new route.
 */
bool reroute(location *start, location *end,
             track_node *route[MAX_ROUTE_LENGTH]);
#endif /* ROUTER_H */
