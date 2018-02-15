#include "command_dispatcher.h"

void command_dispatcher_server() {
  int sender_tid;
  message received;
  Assert(RegisterAs("CommandDispatcher") == 0);
  int train_tx_server = WhoIs("TrainTxServer");
  int clock_server = WhoIs("ClockServer");
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
            Assert(Putc(train_tx_server, TRAIN, CMD_STOP) == 0);
            break;
          case USER_CMD_TR: {
            char to_send_bytes[2];
            to_send_bytes[0] = received.msg.cmd.data[1];
            to_send_bytes[1] = received.msg.cmd.data[0];
            Assert(PutBytes(train_tx_server, to_send_bytes, 2) == 0);
            break;
          }
          case USER_CMD_SW: {
            message send;
            send.type = MESSAGE_SWITCH;
            send.msg.switch_params.clock_server_tid = clock_server;
            send.msg.switch_params.tx_server_tid = train_tx_server;
            send.msg.switch_params.switch_num = (int)received.msg.cmd.data[0];
            send.msg.switch_params.curved = received.msg.cmd.data[1] == 'C';

            int switcher_tid = Create(MyPriority() + 7, &switcher);
            Assert(switcher_tid > 0);
            Assert(Send(switcher_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
            break;
          }
          case USER_CMD_RV:
            // TODO spawn reverser
            break;
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
