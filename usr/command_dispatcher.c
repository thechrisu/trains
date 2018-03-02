#include "command_dispatcher.h"

#define TR_Q_LEN 30

typedef struct {
  int t;
  int tid, msgs_available, msgs_i, msgs_o;
  bool rdy, auto_mode;
  message msgs[TR_Q_LEN];
} conductor_data;

/**
 * Determines if a user command is an automode-command.
 *
 * @param cmd Command
 * @return true if it's an automode command, false if manual mode.
 */
bool is_auto_cmd(user_command *cmd) {
  return cmd->type == USER_CMD_SD;
}

/**
 * Adds m to the conductors' buffer, sends the next message in the buffer
 * if the conductor is ready. If the conductor is not ready,
 * add the message to the buffer anyway.
 *
 * @param m                   Message that we just received.
 * @param c                   Struct of the train conductor, contains messages.
 * @param terminal_tx_server  Tid of the terminal transmit server to print
 *                            error message if in automode.
 */
void send_if_rdy(message *m, conductor_data *c, int terminal_tx_server) {
  if (c->auto_mode) {
    Assert(Printf(terminal_tx_server, "%s%s%s%sAUTOMODE        %s%s",
                  CURSOR_ROW_COL(CMD_LINE, 1), RED_TEXT, HIDE_CURSOR, 
                  HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
    return;
  }
  if (c->msgs_available >= TR_Q_LEN) {
    logprintf("Message queue for train %d full.\n\r", c->t);
  }
  memcpy(&c->msgs[c->msgs_i], m, sizeof(*m));
  c->msgs_i = c->msgs_i < TR_Q_LEN - 1 ? c->msgs_i + 1 : 0;
  c->msgs_available++;
  if (c->rdy) {
    Assert(c->msgs[c->msgs_o].msg.cmd.type < NULL_USER_CMD);
    c->auto_mode = is_auto_cmd(&c->msgs[c->msgs_o].msg.cmd);
    Assert(Send(c->tid, &c->msgs[c->msgs_o], sizeof(c->msgs[c->msgs_i]),
           EMPTY_MESSAGE, 0) == 0);
    c->msgs_o = c->msgs_o < TR_Q_LEN - 1 ? c->msgs_o + 1 : 0;
    c->msgs_available--;
    c->rdy = false;
  }
}

/**
 * To be called after a `MESSAGE_READY` message has been received by the
 * train conductor, indicating that the current command has finished.
 * If we don't have a message, we indicate that we are able to send a message,
 * the next time we have a message to send (it would be sent via sent_if_rdy()).
 * If we DO have a message, we send it immediately; In this case, the
 * train conductor can't receive any more messages until `MESSAGE_READY` has
 * been received, at some later point in the future.
 *
 * @param c Train conductor to be readied/from which message is to be sent.
 */
void conductor_ready(conductor_data *c) {
  if (c->msgs_available) {
    c->auto_mode = is_auto_cmd(&c->msgs[c->msgs_o].msg.cmd);
    Assert(Send(c->tid, c->msgs + c->msgs_o, sizeof(c->msgs[c->msgs_o]),
           EMPTY_MESSAGE, 0) == 0);
    c->msgs_o = c->msgs_o < TR_Q_LEN - 1 ? c->msgs_o + 1 : 0;
    c->msgs_available--;
    c->rdy = false;
  } else {
    c->rdy = true;
    c->auto_mode = false;
  }
}

void command_dispatcher_server() {
  int active_trains[] = {24, 58, 74};
  int num_active_trains = 3;
  int sender_tid;
  message received;
  Assert(RegisterAs("CommandDispatcher") == 0);
  int train_tx_server = WhoIs("TrainTxServer");
  int terminal_tx_server = WhoIs("TerminalTxServer");
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
    train_to_look_after_msg.type = MESSAGE_CONDUCTOR_SETTRAIN;
    train_to_look_after_msg.msg.train = t;
    Assert(Send(conductors[t].tid, &train_to_look_after_msg,
                sizeof(train_to_look_after_msg), EMPTY_MESSAGE, 0) == 0);
    conductors[t].rdy = true;
    conductors[t].msgs_i = 0;
    conductors[t].msgs_o = 0;
    conductors[t].msgs_available = 0;
    conductors[t].auto_mode = false;
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
          case USER_CMD_SD:
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
              send_if_rdy(&received, &conductors[received.msg.cmd.data[0]],
                          terminal_tx_server);
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
        conductor_ready(conductors + received.msg.train);
        // TODO send back a new command immediately, instead of waiting.
        break;
      default:
        Assert(0); // :(
        break;
    }
  }
}
