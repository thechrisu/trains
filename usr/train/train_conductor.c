#include "train_conductor.h"

void conductor_setspeed(int train_tx_server, int track_state_controller, int clock_server,
                        train_data *d, int speed) {
  d->last_speed = d->should_speed;
  d->time_speed_last_changed = Time(clock_server);
  d->should_speed = speed;
  set_train_speed(train_tx_server, track_state_controller,
                  d->train, d->should_speed);
}

void conductor_reverse(int train_tx_server, int track_state_controller, int clock_server,
                       train_data *d) {
  d->last_speed = d->should_speed;
  d->direction = !d->direction;
  stop_and_reverse_train(clock_server, train_tx_server,
                         track_state_controller, d->train);
  d->time_speed_last_changed = Time(clock_server);
}

void train_conductor() {
  int sender_tid;
  message received, ready;
  int train_tx_server = WhoIs("TrainTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int cmd_dispatcher = WhoIs("CommandDispatcher");
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);
  Assert(cmd_dispatcher > 0);

  train_data d;
  d.last_speed = 0;
  d.time_speed_last_changed = Time(clock_server);

  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
  Assert(received.msg.train > 0 && received.msg.train <= 80);
  Assert(received.type == MESSAGE_CONDUCTOR_SETTRAIN);
  d.train = received.msg.train;

  ready.msg.train = d.train;
  ready.type = MESSAGE_READY;
  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
    Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
    switch (received.type) {
      case MESSAGE_USER:
        switch (received.msg.cmd.type) {
          case USER_CMD_SD:
            logprintf("NotYetImplemented(SD for train %d)\n\r", d.train);
            Delay(clock_server, 10000); // Just to test automode/manual mode, you can remove this later.
            break;
          case USER_CMD_TR:
            Assert(received.msg.cmd.data[0] == d.train);
            conductor_setspeed(train_tx_server, track_state_controller, clock_server,
                               &d, received.msg.cmd.data[1]);
            break;
          case USER_CMD_RV:
            Assert(received.msg.cmd.data[0] == d.train);
            conductor_reverse(train_tx_server, track_state_controller, clock_server, &d);
            break;
          default:
            logprintf("Got user cmd message of type %d\n\r", received.msg.cmd.type);
            Assert(0);
            break;
        }
        Assert(Send(cmd_dispatcher, &ready, sizeof(ready), EMPTY_MESSAGE, 0) == 0);
        break;
      default:
        logprintf("Got user cmd message of type %d\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
