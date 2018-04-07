/**
 * @file
 * @brief Message formats.
 */
#ifndef MESSAGE_H
#define MESSAGE_H

#include "groups.h"
#include "model_data.h"
#include "track_node.h"
#include "tstdlib.h"
#include "user_command.h"

#define PRINTF_MESSAGE_BUFFER_SIZE 256
#define MAX_RESERVATIONS_RETURNED 30

enum message_type {
  MESSAGE_NOTIFIER,
  MESSAGE_TIME,
  REPLY_TIME,
  MESSAGE_DELAY,
  MESSAGE_DELAY_UNTIL,
  REPLY_CLOCK_SERVER_OK,
  REPLY_CLOCK_SERVER_ERROR,
  MESSAGE_K3_GET_PARAMS,
  REPLY_K3_GET_PARAMS,
  MESSAGE_K3_DONE,
  MESSAGE_PUTC,
  MESSAGE_GETC,
  REPLY_GETC,
  MESSAGE_PRINTF,
  REPLY_NAMESERVER,
  MESSAGE_USER,
  MESSAGE_REVERSE,
  MESSAGE_SWITCH,
  MESSAGE_GETTRAIN,
  REPLY_GETTRAIN,
  MESSAGE_GETSENSORS,
  REPLY_GETSENSORS,
  MESSAGE_SENSORSRECEIVED,
  MESSAGE_GETTURNOUTS,
  REPLY_GETTURNOUTS,
  MESSAGE_TRAINSETDIRECTION,
  MESSAGE_TRAINSETSPEED,
  MESSAGE_TRAINREVERSED,
  MESSAGE_TURNOUTSWITCHED,
  MESSAGE_CALIB_SD,
  MESSAGE_CALIB_V,
  MESSAGE_GETCONSTANTSPEEDMODEL,
  REPLY_GETCONSTANTSPEEDMODEL,
  MESSAGE_UPDATECONSTANTSPEEDMODEL,
  MESSAGE_GETSTOPPINGDISTANCEMODEL,
  REPLY_GETSTOPPINGDISTANCEMODEL,
  MESSAGE_UPDATESTOPPINGDISTANCEMODEL,
  MESSAGE_GETSTOPPINGTIMEMODEL,
  REPLY_GETSTOPPINGTIMEMODEL,
  MESSAGE_UPDATESTOPPINGTIMEMODEL,
  MESSAGE_GET_ROUTE,
  REPLY_GET_ROUTE_OK,
  REPLY_GET_ROUTE_ERROR,
  MESSAGE_READY,
  MESSAGE_CONDUCTOR_SETTRAIN,
  MESSAGE_MULTICONDUCTOR_SETGROUP,
  MESSAGE_GET_LAST_SENSOR_HIT,
  REPLY_GET_LAST_SENSOR_HIT,
  MESSAGE_UPDATE_COORDS_SPEED,
  MESSAGE_UPDATE_COORDS_REVERSE,
  MESSAGE_UPDATE_COORDS_SENSOR,
  MESSAGE_FORWARD_TURNOUT_STATES,
  MESSAGE_GET_COORDINATES,
  REPLY_GET_COORDINATES,
  MESSAGE_RESERVATION_MAKE,
  REPLY_RESERVATION_MAKE,
  MESSAGE_RESERVATION_DROP,
  REPLY_RESERVATION_DROP,
  MESSAGE_RESERVATION_DROP_ALL,
  MESSAGE_RESERVATION_GET_ALL,
  REPLY_RESERVATION_GET_ALL,
  MESSAGE_T2_START,
  MESSAGE_GET_DESTINATION,
  REPLY_GET_DESTINATION,
  MESSAGE_CONDUCTOR_NOTIFY_REQUEST,
  REPLY_CONDUCTOR_NOTIFY_REQUEST,
  MESSAGE_SUNSET, // We're not killing the task, we're "sunsetting" it.
  MESSAGE_SLEEP,
  MESSAGE_WAKEUP,
  MESSAGE_REQUEST_COORD_UPDATE,
  MESSAGE_PUBLISH,
  MESSAGE_SUBSCRIBE,
  MESSAGE_EVENT,
  MESSAGE_INIT,
  MAX_MESSAGE_TYPE_ID,
};

typedef enum {
  EVENT_SENSOR_TRIGGERED,
  EVENT_TURNOUT_SWITCHED,
  MAX_EVENT_TYPE,
} event_type;

struct evt {
  event_type type;
  union {
    int16_t sensor;
    struct {
      unsigned char number;
      turnout_state state;
    } turnout;
  } body;
};

typedef struct evt event;

typedef struct {
  int32_t delay;
  int32_t delay_count;
} k3_params;

typedef struct {
  char *buf;
  uint32_t size;
} printf_params;

typedef struct {
  int train_tx_server_tid;
  int clock_server_tid;
  int track_state_controller_tid;
  char train_to_reverse;
} message_reverse_params;

typedef struct {
  int clock_server_tid;
  int tx_server_tid;
  int turnout_num;
  bool curved;
} message_switch_params;

typedef struct {
  unsigned int turnout_num;
  turnout_state state;
} message_turnout_switched_params;

typedef struct {
  int train;
  int speed;
} message_calib_sd_params;

typedef struct {
  int train;
} message_calib_v_params;

typedef struct {
  int train;
  int speed;
  unsigned int start;
  unsigned int end;
  int ticks;
} message_ucsm_params;

typedef struct {
  location start;
  location end;
  track_node **route;
} message_get_route_params;

typedef struct {
  unsigned int sensor;
  int ticks;
} reply_get_last_sensor_hit;

typedef struct {
  train_data tr_data;
  uint32_t velocity_model[15];
  uint32_t stopping_distance_model[15];
  reply_get_last_sensor_hit last_sensor;
} message_update_coords;

typedef struct {
  int train;
  track_node *nodes[2];
} message_reservation_request;

typedef struct {
  int reservations[MAX_RESERVATIONS_RETURNED];
  int count;
} message_reservation_get_all_response;

#define MAX_LOCATIONS_TO_OBSERVE 10
#define MULTI_MAX_LOCATIONS_TO_OBSERVE (2 * MAX_LOCATIONS_TO_OBSERVE)

typedef enum {
  GOT_LOST,
  LOCATION_CHANGED, // We hit a sensor
  LOCATION_TO_SWITCH,
  LOCATION_TO_STOP,
  LOCATION_ANY, // Any location where we're not lost
  SPACING,
  LOCATION_SLOWDOWN,
  LOCATION_UNBLOCKED,
  MAX_NOTIFICATION_TYPE,
} coord_notification_type;

typedef struct {
  union {
    location loc;
    int trains[2]; // [0] Is ahead of [1]
  } subject;
  coord_notification_type reason;
  union {
    int distance[2]; // [0]: IS distance, [1]: SHOULD distance
    char switch_to_switch[2]; // 0: switch number, 1: state to switch to.
  } action;
} location_notification;

typedef struct {
  location_notification notifications[MULTI_MAX_LOCATIONS_TO_OBSERVE];
  int num_requests;
  bool drop_existing;
} location_notification_request;

typedef struct {
  int tid;
  struct evt event;
} event_dispatcher_params;

typedef union {
  int32_t reply_time_ticks;
  int32_t message_delay_ticks;
  int32_t message_delay_until_ticks;
  int32_t sleeper_ticks;
  k3_params reply_k3_params;
  char putc;
  char getc;
  char train;
  printf_params printf;
  user_command cmd;
  int nameserver_response;
  message_reverse_params reverser_params;
  message_switch_params switch_params;
  train_data tr_data;
  int16_t sensors[10];
  uint32_t train_speeds[15];
  uint32_t train_distances[15];
  uint32_t train_times[15];
  train_group *group_ptr;
  train_group group_content;
  message_turnout_switched_params turnout_switched_params;
  turnout_state turnout_states[NUM_TURNOUTS];
  message_calib_sd_params calib_sd_params;
  message_calib_v_params calib_v_params;
  message_ucsm_params ucsm;
  default_value usdm;
  default_value ustm;
  message_get_route_params get_route_params;
  reply_get_last_sensor_hit last_sensor;
  coordinates coords;
  message_update_coords update_coords;
  message_reservation_request reservation_request;
  int reservation_response;
  message_reservation_get_all_response all_reservations;
  int destination;
  location_notification_request notification_request;
  location_notification notification_response;
  struct evt event;
  event_dispatcher_params edparams;
} message_body;

typedef struct {
  int type;
  message_body msg;
} message;

#define EMPTY_MESSAGE (message *)0

#endif /* MESSAGE_H */
