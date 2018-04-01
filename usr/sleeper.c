#include "sleeper.h"

void sleeper() {
  int message_tid;
  message duration_ticks_msg;
  int master = MyParentTid();
  int clock_server = WhoIs("ClockServer");

  Assert(Receive(&message_tid, &duration_ticks_msg, sizeof(duration_ticks_msg))
      == sizeof(duration_ticks_msg));
  Assert(message_tid == master);
  Assert(Reply(message_tid, EMPTY_MESSAGE, 0) == 0);

  Assert(Delay(clock_server, duration_ticks_msg.msg.sleeper_ticks) == 0);

  message reply_wakeup;
  reply_wakeup.type = MESSAGE_WAKEUP;
  Assert(Send(master, &reply_wakeup, sizeof(reply_wakeup), EMPTY_MESSAGE, 0) == 0);
}

void set_alarm(int ticks) {
  int sleeper_tid = Create(MyPriority(), &sleeper);
  Assert(sleeper_tid > 0);
  message duration_sleep_ticks;
  duration_sleep_ticks.type = MESSAGE_SLEEP;
  duration_sleep_ticks.msg.sleeper_ticks = (int32_t)ticks;
  Assert(Send(sleeper_tid, &duration_sleep_ticks, sizeof(duration_sleep_ticks), EMPTY_MESSAGE, 0) == 0);
}
