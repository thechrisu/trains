#include "t2_demo_task.h"

static long long seed;

int random() {
  long long m = 0x10000;
  long long a = 1103515245;
  long long c = 12345;
  seed = (a * seed + c) % m;
  return seed % m;
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
    send.msg.cmd.data[1] = random() % 80;
    send.msg.cmd.data[2] = 0;

    Assert(Send(command_dispatcher, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
  }
}
