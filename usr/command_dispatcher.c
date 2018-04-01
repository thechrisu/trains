#include "command_dispatcher.h"

#define TR_Q_LEN 30

typedef struct {
  int t;
  int tid, msgs_available, msgs_i, msgs_o;
  bool rdy, auto_mode;
  message msgs[TR_Q_LEN];
  int t2_tid;
  int destination;
} conductor_data;

/**
 * Determines if a user command is an automode-command.
 *
 * @param cmd Command
 * @return true if it's an automode command, false if manual mode.
 */
bool is_auto_cmd(user_command *cmd) {
  return cmd->type == USER_CMD_SD ||
         cmd->type == USER_CMD_R ||
         cmd->type == USER_CMD_LOOP;
}

/**
 * Prints out the present train groupings
 *
 * @param terminal_tx_server
 */
void print_groups(int terminal_tx_server) {
  for (int i = 0; i < MAX_GROUPS; i++) {
    if (i < num_groups) {
      switch (tr_groups[i].g.num_members) {
        case 1:
          Assert(Printf(terminal_tx_server, "%s%d;%dH#%d: %s\t%d%s%s",
                        ESC, GROUP_LINE + i, 1, i + 1, tr_groups[i].group_name,
                        tr_groups[i].g.members[0], HIDE_CURSOR, HIDE_CURSOR_TO_EOL
                        ) == 0);
          break;
        case 2:
          Assert(Printf(terminal_tx_server, "%s%d;%dH#%d: %s\t%d\t%d%s%s",
                        ESC, GROUP_LINE + i, 1, i + 1, tr_groups[i].group_name,
                        tr_groups[i].g.members[0],
                        tr_groups[i].g.members[1], HIDE_CURSOR, HIDE_CURSOR_TO_EOL
              ) == 0);
          break;
        case 3:
          Assert(Printf(terminal_tx_server, "%s%d;%dH#%d: %s\t%d\t%d\t%d%s%s",
                        ESC, GROUP_LINE + i, 1, i + 1, tr_groups[i].group_name,
                        tr_groups[i].g.members[0],
                        tr_groups[i].g.members[1],
                        tr_groups[i].g.members[2], HIDE_CURSOR, HIDE_CURSOR_TO_EOL
              ) == 0);
          break;
        case 4:
          Assert(Printf(terminal_tx_server, "%s%d;%dH#%d: %s\t%d\t%d\t%d\t%d%s%s",
                        ESC, GROUP_LINE + i, 1, i + 1, tr_groups[i].group_name,
                        tr_groups[i].g.members[0],
                        tr_groups[i].g.members[1],
                        tr_groups[i].g.members[2],
                        tr_groups[i].g.members[3], HIDE_CURSOR, HIDE_CURSOR_TO_EOL
              ) == 0);
          break;
        case 5:
          Assert(Printf(terminal_tx_server, "%s%d;%dH#%d: %s\t%d\t%d\t%d\t%d\t%d%s%s",
                        ESC, GROUP_LINE + i, 1, i + 1, tr_groups[i].group_name,
                        tr_groups[i].g.members[0],
                        tr_groups[i].g.members[1],
                        tr_groups[i].g.members[2],
                        tr_groups[i].g.members[3],
                        tr_groups[i].g.members[4], HIDE_CURSOR, HIDE_CURSOR_TO_EOL
              ) == 0);
          break;
        case 6:
          Assert(Printf(terminal_tx_server, "%s%d;%dH#%d: %s\t%d\t%d\t%d\t%d\t%d\t%d%s%s",
                        ESC, GROUP_LINE + i, 1, i + 1, tr_groups[i].group_name,
                        tr_groups[i].g.members[0],
                        tr_groups[i].g.members[1],
                        tr_groups[i].g.members[2],
                        tr_groups[i].g.members[3],
                        tr_groups[i].g.members[4],
                        tr_groups[i].g.members[5], HIDE_CURSOR, HIDE_CURSOR_TO_EOL
              ) == 0);
          break;
      }
    } else {
      Assert(Printf(terminal_tx_server, "%s%d;%dH%s%s",
                    ESC, GROUP_LINE + i, 1, HIDE_CURSOR,
                    HIDE_CURSOR_TO_EOL) == 0);
    }
  }
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
    Assert(Printf(terminal_tx_server, "%s%s%sAUTOMODE        %s%s",
                  CURSOR_ROW_COL(CMD_LINE, 1), RED_TEXT, HIDE_CURSOR,
                  HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
    return;
  }
  if (c->msgs_available >= TR_Q_LEN) {
    logprintf("Message queue for train %d full.\n\r", c->t);
  }
  tmemcpy(&c->msgs[c->msgs_i], m, sizeof(*m));
  c->msgs_i = c->msgs_i < TR_Q_LEN - 1 ? c->msgs_i + 1 : 0;
  c->msgs_available++;
  if (c->rdy) {
    message *msg = &c->msgs[c->msgs_o];
    if (msg->msg.cmd.type == USER_CMD_R) {
      c->destination = msg->msg.cmd.data[1];
    }

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
    message *msg = &c->msgs[c->msgs_o];
    if (msg->msg.cmd.type == USER_CMD_R) {
      c->destination = msg->msg.cmd.data[1];
    }

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

/**
 * Creates a train conductor task and tells it what train to supervise.
 *
 * @param t                                  Train
 * @param my_priority                        Priority of the command_dispatcher
 * @param conductors                         Array to put initialized conductor info into.
 */
void create_conductor(int t, int my_priority, conductor_data conductors[81]) {
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
  conductors[t].destination = NO_DESTINATION;
  conductors[t].t2_tid = 0;
}

/**
 * Much better than 'Killing' a task:
 * We tell the task to kill itself, and clean up after itself while doing so.
 *
 * @param tid                      Task to sunset.
 */
void sunset_tid(int tid) {
  if (tid > 0) {
    message sunset;
    sunset.type = MESSAGE_SUNSET;
    Assert(Send(tid, &sunset, sizeof(sunset),
          EMPTY_MESSAGE, 0) == 0);
  }
}

void update_conductors(conductor_data conductors[81]) {
  int my_priority = MyPriority();

  for (int i = 0; i < num_active_trains; i++) {
    int t = active_trains[i];

    if (conductors[t].tid == 0) {
      create_conductor(t, my_priority, conductors);
    }
  }
}

void command_dispatcher_server() {
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
  Assert(terminal_tx_server > 0);

  conductor_data conductors[81];
  for (int i = 0; i < 81; i += 1) {
    conductors[i].tid = 0;
  }
  update_conductors(conductors);

  num_groups = 0;
  while (true) {
    Assert(Receive(&sender_tid, &received, sizeof(received)) >= 0);

    if (received.type != MESSAGE_GET_DESTINATION &&
        !(received.type == MESSAGE_USER &&
          received.msg.cmd.type == USER_CMD_R &&
          sender_tid == conductors[received.msg.cmd.data[0]].t2_tid)) {
      Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) >= 0);
    }

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
          case USER_CMD_LOOP:
          case USER_CMD_RV: // "block" on command
          case USER_CMD_R:
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
          case USER_CMD_T2START: {
            int train = (int)received.msg.cmd.data[0];

            if (conductors[train].t2_tid == 0) {
              message send;
              send.type = MESSAGE_T2_START;
              send.msg.train = train;

              int t2_tid = Create(MyPriority() + 1, &t2_demo_task);
              Assert(t2_tid > 0);
              conductors[train].t2_tid = t2_tid;
              Assert(Send(t2_tid, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
            }

            break;
          }
          case USER_CMD_T2STOP: {
            int train = (int)received.msg.cmd.data[0];
            int t2_tid = conductors[train].t2_tid;

            if (t2_tid != 0) {
              Assert(Kill(t2_tid) == 0);
              conductors[train].t2_tid = 0;
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
              case SET_TRAINS:
                num_active_trains = received.msg.cmd.data[1];
                for (int i = 0; i < num_active_trains; i += 1) {
                  active_trains[i] = received.msg.cmd.data[i + 2];
                }
                update_conductors(conductors);
                break;
              case SET_SWITCH_PADDING:
                switch_padding = received.msg.cmd.data[1];
                break;
              case SET_TRACK:
                if (received.msg.cmd.data[1] == 0) {
                  track.track = track.tracka;
                  track.nodes = TRACKA_SIZE;
                } else {
                  track.track = track.trackb;
                  track.nodes = TRACKB_SIZE;
                }
                break;
              case SET_DIRECTION: {
                int train = received.msg.cmd.data[1];
                bool is_forward = (bool)received.msg.cmd.data[2]; // any != 0
                message send;
                send.type = MESSAGE_TRAINSETDIRECTION;
                send.msg.tr_data.train = train;
                send.msg.tr_data.direction = is_forward;
                Assert(Send(track_state_controller, &send, sizeof(send), EMPTY_MESSAGE, 0) == 0);
                break;
              }
              default:
                Assert(0);
                break;
            }
            break;
          case USER_CMD_GROUP: {
            if (num_groups >= MAX_GROUPS) {
              logprintf("Max number of groups already reached, can't add new group (cmd dispatcher)\n\r");
              break;
            }
            int group_name_len = (int)received.msg.cmd.data[0];
            Assert(group_name_len <= MAX_GROUP_NAME_LEN);
            char *group_name = (char *)received.msg.cmd.data[1];
            int num_members = (int)received.msg.cmd.data[2];
            Assert(num_members <= MAX_GROUP_MEMBERS);

            tr_groups[num_groups].g.num_members = num_members;
            tmemcpy(tr_groups[num_groups].group_name, group_name, MAX_GROUP_NAME_LEN);
            for (int i = 0; i < num_members; i++) {
              int tr = received.msg.cmd.data[3 + i];
              tr_groups[num_groups].g.members[i] = tr;
              sunset_tid(conductors[tr].tid);
              // Need to do this, since it may be polling (conductor_loop)
              // For some reason, if you uncomment the Kill(),
              // we end up in an infinite loop. Debugging this is super hard
              // --> screw debugging that, we just won't group
              // before exiting from conductor_loop()
              // Kill(conductors[tr].tid);
            }
            tr_groups[num_groups].tid = Create(my_priority + 1,
                                               &multi_train_conductor);
            Assert(tr_groups[num_groups].tid > 0);
            message setgroup;
            setgroup.type = MESSAGE_MULTICONDUCTOR_SETGROUP;
            tmemcpy(&setgroup.msg.group_content, &tr_groups[num_groups].g,
                sizeof(tr_groups[num_groups].g));
            Assert(Send(tr_groups[num_groups].tid, &setgroup, sizeof(setgroup),
                  EMPTY_MESSAGE, 0) == 0);
            num_groups += 1;
            print_groups(terminal_tx_server);
            break;
          }
          case USER_CMD_UNGROUP: {
            int index = -1;
            for (int i = 0; i < num_groups; i++) {
              if (tstrcmp(tr_groups[i].group_name, (char *)received.msg.cmd.data[0])) {
                index = i;
                break;
              }
            }
            if (index != -1) {
              sunset_tid(tr_groups[index].tid);
              for (int i = 0; i < tr_groups[index].g.num_members; i++) {
                create_conductor(tr_groups[index].g.members[i], my_priority, conductors);
              }
              for (int i = index + 1; i < num_groups; i++) {
                tmemcpy(tr_groups + i - 1, tr_groups + i, sizeof(tr_groups[index]));
              }
              num_groups -= 1;
              print_groups(terminal_tx_server);
            } else {
              logprintf("Should have found group %s\n\r", received.msg.cmd.data[0]);
            }
            break;
          }
          default:
            Assert(0);
            break; // Invalid command.
        }
        break;
      }
      case MESSAGE_GET_DESTINATION: {
        message reply;
        reply.type = REPLY_GET_DESTINATION;
        reply.msg.destination = conductors[(int)received.msg.train].destination;
        Assert(Reply(sender_tid, &reply, sizeof(reply)) == 0);
        break;
      }
      case MESSAGE_READY:
        conductor_ready(conductors + received.msg.train);

        int t2_tid = conductors[(int)received.msg.train].t2_tid;
        if (t2_tid != 0) {
          Assert(Reply(t2_tid, EMPTY_MESSAGE, 0) >= 0);
        }

        conductors[(int)received.msg.train].destination = NO_DESTINATION;
        break;
      default:
        Assert(0); // :(
        break;
    }
  }
}
