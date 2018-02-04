/**
 * @file
 * @brief Functions that modify the model of the track
 */
#ifndef A0_TRACKSTATE_H
#define A0_TRACKSTATE_H

extern struct track_state global_track_state;
extern int oe_in_sensor;

#include "a0terminal.h"
#include "assert.h"
#include "char_buffer.h"
#include "mytimer.h"
#include "stdlib.h"
#include "../track/track_node.h"
#include "../track/track_data.h"

void blink_headlights(char loco);

void set_speed(int train, int speed);
void set_headlights(int train, bool turn_on);
void set_turnout(int turnout_num, char state);
void reverse(int train);
void turn_off_solenoid_if_necessary(uint32_t timestamp);
void check_reverse(uint32_t timestamp);
void check_turnout_buffer();
void query_sensors();
void setup_turnouts();
void check_sensors(char *sensor_state, char_buffer *recentSensorsBuf, uint32_t timestamp);

void set_go(bool go);

#endif /* A0_TRACKSTATE_H */
