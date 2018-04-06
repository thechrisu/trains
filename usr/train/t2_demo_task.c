#include "t2_demo_task.h"

static long long seed;

int random() {
  long long m = 0x10000;
  long long a = 1103515245;
  long long c = 12345;
  seed = (a * seed + c) % m;
  return seed % m;
}

int random_sensor_on_siding() {
  char sensors[][4] = {
    "A15",
    "A13",
    "A1",
    "B5",
    "D3",
    "E5",
    "E15",
    "E3",
    "E1",
    "D1",
    "C1",
    "B13",
    "B3",
    "D15",
    "B1",
    "D13",
    "C7",
    "C3",
    "A5",
    "A7",
    "B9",
    "B11",
    "B7",
    "A11",
    "A9"
  };
  int num_sensors = 25;
  return find_node_by_name(&track, sensors[random() % num_sensors])->num;
}

void t2_demo_task() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);

  int command_dispatcher = WhoIs("CommandDispatcher");

  seed = get_clockticks();

  while (true) {
    message send;

    send.type = MESSAGE_USER;
    send.msg.cmd.type = USER_CMD_R;
    send.msg.cmd.data[0] = received.msg.train;
    send.msg.cmd.data[1] = random_sensor_on_siding();
    send.msg.cmd.data[2] = 0;

    Assert(Send(command_dispatcher, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
  }
}
