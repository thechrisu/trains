#include "command_dispatcher.h"

void command_dispatcher_server() {
  int sender_tid;
  message received, train_data_msg;
  Assert(RegisterAs("CommandDispatcher") == 0);
  int train_tx_server = WhoIs("TrainTxServer");
  int clock_server = WhoIs("ClockServer");
  int track_state_controller = WhoIs("TrackStateController");
  int my_priority = MyPriority();
  Assert(train_tx_server > 0);
  Assert(clock_server > 0);
  Assert(track_state_controller > 0);

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
          case USER_CMD_TR: {
            char to_send_bytes[2];
            to_send_bytes[0] = received.msg.cmd.data[1];
            to_send_bytes[1] = received.msg.cmd.data[0];
            Assert(PutBytes(train_tx_server, to_send_bytes, 2) == 0);

            train_data_msg.type = MESSAGE_TRAINSETSPEED;
            train_data_msg.msg.train = received.msg.cmd.data[0];
            train_data_msg.msg.tr_data.should_speed = received.msg.cmd.data[1];
            Assert(Send(track_state_controller, &train_data_msg, sizeof(train_data_msg),
                        EMPTY_MESSAGE, 0) >= 0);
            break;
          }
          case USER_CMD_SW: {
            int turnout_num = (int)received.msg.cmd.data[0];
            int curved = received.msg.cmd.data[1] == 'C';

            message send;
            send.type = MESSAGE_SWITCH;
            send.msg.switch_params.clock_server_tid = clock_server;
            send.msg.switch_params.tx_server_tid = train_tx_server;
            send.msg.switch_params.switch_num = turnout_num;
            send.msg.switch_params.curved = curved;

            int switcher_tid = Create(MyPriority() + 7, &switcher);
            Assert(switcher_tid > 0);
            Assert(Send(switcher_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);

            train_data_msg.type = MESSAGE_TURNOUTSWITCHED;
            train_data_msg.msg.turnout_switched_params.turnout_num = turnout_num;
            train_data_msg.msg.tr_data.should_speed = curved ? TURNOUT_CURVED : TURNOUT_STRAIGHT;
            Assert(Send(track_state_controller, &train_data_msg, sizeof(train_data_msg),
                        EMPTY_MESSAGE, 0) >= 0);
            break;
          }
          case USER_CMD_RV: {
            message rv_cmd;
            rv_cmd.type = MESSAGE_REVERSE;
            rv_cmd.msg.reverser_params.train_to_reverse = received.msg.cmd.data[0];
            rv_cmd.msg.reverser_params.train_tx_server_tid = train_tx_server;
            rv_cmd.msg.reverser_params.clock_server_tid = clock_server;
            rv_cmd.msg.reverser_params.track_state_controller_tid = track_state_controller;
            int reverser_tid = Create(my_priority + 7, &reverser);
            Send(reverser_tid, &rv_cmd, sizeof(rv_cmd), EMPTY_MESSAGE, 0);
            break;
          }
          default:
            Assert(0);
            break; // Invalid command.
        }
        break;
      }
      default:
        Assert(0); // :(
        break;
    }
  }
}
