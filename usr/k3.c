#include "k3.h"

int clock_server_tid;

void k3_client() {
  message send, reply;
  int my_tid = MyTid();
  int my_parent_tid = MyParentTid();
  int32_t delay, delay_count;

  send.type = MESSAGE_K3_GET_PARAMS;
  Assert(Send(my_parent_tid, &send, sizeof(send), &reply, sizeof(reply)) == sizeof(reply));

  delay = reply.msg.reply_k3_params.delay;
  delay_count = reply.msg.reply_k3_params.delay_count;

  for (int i = 1; i <= delay_count; i += 1) {
    Assert(Delay(clock_server_tid, delay) == 0);
    bwprintf(
      "Tid: %d\tDelay interval: %d\t%s%d out of %s%d delays completed\tDelayed %s%d ticks so far\n\r",
      my_tid,
      delay,
      i < 10 ? " " : "",
      i,
      delay_count < 10 ? " " : "",
      delay_count,
      i * delay < 100 ? " " : "",
      i * delay
    );
  }

  send.type = MESSAGE_K3_DONE;
  Assert(Send(my_parent_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
}

void k3_first_user_task() {
  int32_t delays[4] = { 10, 23, 33, 71 };
  int32_t delay_counts[4] = { 20, 9, 6, 3 };

  int sender_tid;
  int my_priority = MyPriority();

  message receive, reply;
  reply.type = REPLY_K3_GET_PARAMS;

  EnableCaches(true);

#ifdef E2ETESTING
  ns_tid = Create(my_priority - 5, &nameserver_main);
  Assert(ns_tid > 0);
#else
  Assert(Create(my_priority - 5, &nameserver_main) > 0);
#endif
  Assert(Create(my_priority - 10, &idle_task) > 0);
  clock_server_tid = Create(my_priority - 5, &clock_server);
  Assert(clock_server_tid > 0);

  for (int i = 1; i <= 4; i += 1) {
    Assert(Create(my_priority - 10 + i, &k3_client) > 0);
  }

  // Send parameters to clients.
  for (int i = 0; i < 4; i += 1) {
    Assert(Receive(&sender_tid, &receive, sizeof(receive)) == sizeof(receive));
    Assert(receive.type == MESSAGE_K3_GET_PARAMS);
    reply.msg.reply_k3_params.delay = delays[i];
    reply.msg.reply_k3_params.delay_count = delay_counts[i];
    Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);;
  }

  // Wait until all clients are ready to exit before exiting.
  for (int i = 0; i < 4; i += 1) {
    Assert(Receive(&sender_tid, &receive, sizeof(receive)) == sizeof(receive));
    Assert(receive.type == MESSAGE_K3_DONE);
    Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
  }

  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(WhoIs("Idle")) == 0);
}
