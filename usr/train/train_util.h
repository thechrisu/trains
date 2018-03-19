#ifndef TRAIN_UTIL_H
#define TRAIN_UTIL_H

#include "commands.h"
#include "global_track_state.h"
#include "prediction.h"
#include "terminal.h"
#include "tstdlib.h"

/**
 * @param old_sensors  The previous sensor data.
 * @param new_sensors  The current sensor data.
 * @param leading_edge A buffer in which to store information about which sensors were triggered this sensor read.
 */
void get_leading_edge(int16_t old_sensors[10], int16_t new_sensors[10], int16_t leading_edge[10]);

/**
 * @param   sensors A set of sensor data.
 * @param   offset  The offset of a sensor within the sensor data bitmap.
 * @returns Whether or not the sensor is triggered in the sensor data.
 */
bool is_sensor_triggered(int16_t sensors[10], unsigned int offset);

/**
 * Poll the track state controller for sensor data until either
 * 1) the given sensor has been triggered or
 * 2) the number of ticks spent polling exceed the timeout.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param offset                     The offset of a sensor within the sensor data bitmap.
 * @param timeout_ticks              Ticks until we return.
 * @return                           1 if it timed out, 0 if the sensor was received.
 */
int poll_until_sensor_triggered_with_timeout(int clock_server_tid,
                                             int track_state_controller_tid,
                                             unsigned int offset,
                                             int timeout_ticks);

/**
 * Poll the track state controller for sensor data until either
 * 1) the given sensor has been triggered,
 * 2) the pair of the given sensor has been triggered, or
 * 3) the number of ticks spent polling exceed the timeout.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param offset                     The offset of a sensor within the sensor data bitmap.
 * @param timeout_ticks              Ticks until we return.
 * @return                           `true` if it timed out, `false` if the sensor was received.
 */
bool poll_until_sensor_pair_triggered_with_timeout(int clock_server_tid,
                                                   int track_state_controller_tid,
                                                   unsigned int offset,
                                                   int timeout_ticks);

/**
 * Poll the track state controller for sensor data until the given sensor has been triggered.
 *
 * @param clock_server_tid           The task ID of the clock server.
 * @param track_state_controller_tid The task ID of the track state controller.
 * @param offset                     The offset of a sensor within the sensor data bitmap.
 */
void poll_until_sensor_triggered(int clock_server_tid, int track_state_controller_tid, unsigned int offset);

/**
 * Blocks until the train has travelled dist.
 *
 * @param clock_server             Tid of the clock server.
 * @param terminal_tx_server       Tid of the terminal transmit server.
 * @param dist                     Distance to travel ('1' is 1/100thm).
 * @param velocity                 Velocity of the train (1/100thmm/s).
 */
void poll_until_at_dist(int clock_server, int terminal_tx_server,
                  int dist, int velocity);

/**
 * Given a point in a route, switches all turnouts on the route within a given
 * distance of the point.
 *
 * @param clock_server            Tid of the clock server.
 * @param train_tx_server         Tid of the train tx server to send sw commands.
 * @param track_state_controller  Tid of the track state controller.
 * @param route                   The route.
 * @param loc                     Point in route after which turnouts should be switched.
 * @param distance                Distance inside of which turnouts should be switched.
 */
void switch_turnouts_within_distance(int clock_server, int train_tx_server,
                                     int track_state_controller,
                                     track_node *route[MAX_ROUTE_LENGTH],
                                     location *loc,
                                     int distance);

/**
 * Estimates the stopping distance from a stopping distance model.
 * @param velocity in 1/100mm / s
 * @return stopping distance (1/100 mm)
 */
int get_stopping_distance_from_velocity(int velocity);

/**
 * When stopping, get the remaining distance (1/100mm) until we have stopped.
 *
 * @param speed                     Speed of the train (0-14).
 * @param train                     Train number (1-80).
 * @param ticks_left                Estimate of when we will stop.
 * @return Remaining distance until we have stopped.
 */
int stopping_dist_remaining_dist(int train, int speed, int ticks_left);

/**
 * @param   route A route.
 * @param   loc   A location.
 * @returns Whether the location is on the route.
 */
bool on_route(track_node *route[MAX_ROUTE_LENGTH], location *loc);

/**
  * Iterates through the route until the end.
  *
  * @param  route A route.
  * @param  loc   The location to start measuring the distance from.
  * @return The remaining distance in 1/100mm.
  */
int get_remaining_dist_in_route(track_node *route[MAX_ROUTE_LENGTH], location *loc);

/**
 * Given a route, returns either
 * the first node in the suffix of the route that is of a certain type
 * (e.g. NODE_SENSOR), or will return NULL_RESERVATION, if none found.
 *
 * @param remaining_route      Suffix of the route.
 * @param type                 e.g. NODE_SENSOR.
 * @return next node of type, or NULL_RESERVATION if no such node exists.
 */
track_node **get_next_of_type(track_node **remaining_route, node_type type);

/**
 * @param   route The route.
 * @param   loc   The location to measure from.
 * @param   end   The point on the route to measure to.
 * @returns The distance from `loc` to `end` in 1/100 mm.
 */
int get_dist_on_route(track_node *route[MAX_ROUTE_LENGTH], location *loc, track_node **end);

#endif /* TRAIN_UTIL_H */
