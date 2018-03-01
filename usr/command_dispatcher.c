#include "command_dispatcher.h"

#define TR_Q_LEN 30

typedef struct {
  int t;
  int tid, msgs_available, msgs_i, msgs_o;
  bool rdy;
  message msgs[TR_Q_LEN];
} conductor_data;

void send_if_rdy(message *m, conductor_data *c) {
  if (c->msgs_available >= TR_Q_LEN)
    logprintf("Message queue for train %d full.\n\r", c->t);
  Assert(sizeof(c->msgs[c->msgs_i]) == sizeof(*m));
  memcpy(c->msgs + c->msgs_i, m, sizeof(*m));
  c->msgs_i = c->msgs_i < TR_Q_LEN - 1 ? c->msgs_i + 1 : 0;
  c->msgs_available++;
  if (c->rdy) {
    Assert(c->msgs[c->msgs_o].type < NULL_USER_CMD);
    Assert(Send(c->tid, c->msgs + c->msgs_o, sizeof(c->msgs[c->msgs_i]),
           EMPTY_MESSAGE, 0) == 0);
    c->msgs_o = c->msgs_o < TR_Q_LEN - 1 ? c->msgs_o + 1 : 0;
    c->msgs_available--;
    c->rdy = false;
  }
}

void conductor_ready(conductor_data *c) {
  if (c->msgs_available) {
    logprintf("conductor ready, before message\n\r");
    Assert(Send(c->tid, c->msgs + c->msgs_o, sizeof(c->msgs[c->msgs_o]),
           EMPTY_MESSAGE, 0) == 0);
    logprintf("conductor ready, after message\n\r");
    c->msgs_o = c->msgs_o < TR_Q_LEN - 1 ? c->msgs_o + 1 : 0;
    c->msgs_available--;
    c->rdy = false;
  } else {
    logprintf("conductor !ready\n\r");
    c->rdy = true;
  }
}

void command_dispatcher_server() {
  int active_trains[] = {24, 58, 74};
  int num_active_trains = 3;
  int sender_tid;
  message received;
  Assert(RegisterAs("CommandDispatcher") == 0);
  int train_tx_server = WhoIs("TrainTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int my_priority = MyPriority();
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);

  conductor_data conductors[81];
  for (int i = 0; i < num_active_trains; i++) {
    int t = active_trains[i];
    conductors[t].t = t;
    conductors[t].tid = Create(my_priority + 1, &train_conductor);
    Assert(conductors[t].tid > 0);
    message train_to_look_after_msg;
    train_to_look_after_msg.type = 1337;
    train_to_look_after_msg.msg.train = t;
    logprintf("Before assigning train %d\n\r", t);
    Assert(Send(conductors[t].tid, &train_to_look_after_msg,
                sizeof(train_to_look_after_msg), EMPTY_MESSAGE, 0) == 0);
    logprintf("After assigning train %d\n\r", t);
    conductors[t].rdy = true;
    conductors[t].msgs_i = 0;
    conductors[t].msgs_o = 0;
    conductors[t].msgs_available = 0;
}

while (true) {
  Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
  switch (received.type) {
    case MESSAGE_USER: {
      switch (received.msg.cmd.type) {
        case USER_CMD_GO:
          Assert(Putc(train_tx_server, TRAIN, CMD_GO) == 0);
          break;
        case USER_CMD_STOP:
        case USER_CMD_Q:
          Assert(Putc(train_tx_server, TRAIN, CMD_STOP) == 0);
          break;
        case USER_CMD_RV: // "block" on command
        case USER_CMD_TR: {
          int there = 0;
          for (int i = 0; i < num_active_trains; i++) {
            if (active_trains[i] == received.msg.cmd.data[0]) {
              there = active_trains[i];
              break;
            }
          }
          if (there) {
            Assert(there == (*(conductors + received.msg.cmd.data[0])).t);
            send_if_rdy(&received, conductors + received.msg.cmd.data[0]);
          }
          break;
        }
        case USER_CMD_SW: {
          int turnout_num = (int)received.msg.cmd.data[0];
          int curved = received.msg.cmd.data[1] == 'C';

          message send;
          send.type = MESSAGE_SWITCH;
          send.msg.switch_params.clock_server_tid = clock_server;
          send.msg.switch_params.tx_server_tid = train_tx_server;
          send.msg.switch_params.turnout_num = turnout_num;
          send.msg.switch_params.curved = curved;

          int switcher_tid = Create(MyPriority() + 7, &switcher);
          Assert(switcher_tid > 0);
          Assert(Send(switcher_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
          break;
        }
        case USER_CMD_SD: {
          message send;
          send.type = MESSAGE_CALIB_SD;
          send.msg.calib_sd_params.train = (int)received.msg.cmd.data[0];
          send.msg.calib_sd_params.speed = (int)received.msg.cmd.data[1];

          int child_tid = Create(my_priority + 7, &stopping_distance_calibrator);
          Assert(Send(child_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
          break;
        }
        case USER_CMD_V: {
          message send;
          send.type = MESSAGE_CALIB_V;
          send.msg.calib_v_params.train = (int)received.msg.cmd.data[0];

          int child_tid = Create(my_priority + 7, &automated_velocity_calibrator);
          Assert(Send(child_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
          break;
        }
        case USER_CMD_SET:
          switch (received.msg.cmd.data[0]) {
            case SET_T1TRAIN:
              t1train = received.msg.cmd.data[1];
              break;
            default:
              Assert(0);
                break;
            }
            break;
          default:
            Assert(0);
            break; // Invalid command.
        }
        break;
      }
      case MESSAGE_READY:
        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
        conductor_ready(conductors + received.msg.train);
        // TODO send back a new command immediately, instead of waiting.
        break;
      default:
        Assert(0); // :(
        break;
    }
  }
}
