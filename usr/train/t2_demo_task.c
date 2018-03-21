#include "t2_demo_task.h"

static int seed;

int random_sensor() {
  int m = 80;
  int a = 1103515251;
  int c = 44701;
  seed = (a * seed + c) % m;
  return seed;
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

    send.type = MESSAGE_USER;
    send.msg.cmd.type = USER_CMD_R;
    send.msg.cmd.data[0] = received.msg.train;
    send.msg.cmd.data[1] = random_sensor();
    send.msg.cmd.data[2] = 0;

    Assert(Send(command_dispatcher, &send, sizeof(send), EMPTY_MESSAGE, 0) == sizeof(send));
  }
}
