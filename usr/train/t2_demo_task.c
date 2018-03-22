#include "t2_demo_task.h"

static long long seed;

int random_sensor() {
  long long m = 80;
  long long a = 1103515251;
  long long c = 44701;
  seed = (a * seed + c) % m;
  return seed % m;
}

void t2_demo_task() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

  int command_dispatcher = WhoIs("CommandDispatcher");
  int train_tx_server_tid = WhoIs("TrainTxServer");
  int track_state_controller_tid = WhoIs("TrackStateController");

  seed = get_clockticks();

  while (true) {
    message send;

    set_train_speed(train_tx_server_tid, track_state_controller_tid, received.msg.train, 12);

    int sensor = random_sensor();
    // TODO remove this check after routing with reversal is sufficiently robust
    while (!sensor_reachable(&track, sensor, sensor_offset('D', 5)) ||
           !sensor_reachable(&track, sensor_pair(&track, sensor), sensor_offset('D', 5))) {
      sensor = random_sensor();
    }

    send.type = MESSAGE_USER;
    send.msg.cmd.type = USER_CMD_R;
    send.msg.cmd.data[0] = received.msg.train;
    send.msg.cmd.data[1] = sensor;
    send.msg.cmd.data[2] = 0;

    Assert(Send(command_dispatcher, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
  }
}
