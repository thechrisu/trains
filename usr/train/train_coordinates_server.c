#include "train_coordinates_server.h"

void train_coordinates_server() {
  Assert(RegisterAs("TrainCoordinatesServer") == 0);

  int sender_tid;
  message received, reply;

  int clock_server = WhoIs("ClockServer");

  coordinates coords[80];
  for (int i = 0; i < 80; i += 1) {
    coords[i].loc.sensor = NO_NEXT_SENSOR;
  }

  turnout_state turnout_states[NUM_TURNOUTS];

  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    message_update_coords *uc = &received.msg.update_coords;
    coordinates *train_coords = &coords[(int)uc->tr_data.train];
    switch (received.type) {
      case MESSAGE_UPDATE_COORDS_SPEED:
        update_coordinates_after_speed_change(&uc->tr_data, uc->velocity_model,
                                              uc->acceleration, turnout_states,
                                              train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_UPDATE_COORDS_REVERSE:
        update_coordinates_after_reverse(train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_UPDATE_COORDS_SENSOR:
        update_coordinates_after_sensor_hit(&uc->last_sensor, turnout_states,
                                            train_coords);
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_FORWARD_TURNOUT_STATES:
        tmemcpy(turnout_states, &received.msg.turnout_states,
                NUM_TURNOUTS * sizeof(turnout_state));
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      case MESSAGE_GET_COORDINATES: {
        coordinates *c = &coords[(int)received.msg.train];
        update_coordinates_after_time_passed(clock_server, turnout_states, c);
        reply.type = REPLY_GET_COORDINATES;
        tmemcpy(&(reply.msg.coords), c, sizeof(coordinates));
        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      default:
        logprintf("Received message of type %d in train coordinates server\n\r", received.type);
        Assert(0);
    }
  }
}
