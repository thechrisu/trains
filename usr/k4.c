#include "k4.h"

#define CMD_MAX_SZ 32
#define CMD_LINE 2

enum user_command_type {
  USER_CMD_GO,
  USER_CMD_STOP,
  USER_CMD_TR,
  USER_CMD_SW,
  USER_CMD_RV,
  USER_CMD_Q,
  NULL_USER_CMD
};

typedef struct {
  enum user_command_type type;
  int data[2];
} user_command;

void user_command_reset(user_command *cmd) {
  cmd->type = NULL_USER_CMD;
  cmd->data[0] = 0;
  cmd->data[1] = 0;
}

void user_command_print(int server_tid, user_command *cmd) {
  switch(cmd->type) {
    case USER_CMD_GO:
      Printf(server_tid, "%s%sGO%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1), HIDE_CURSOR_TO_EOL);
      break;
    case USER_CMD_STOP:
      Printf(server_tid, "%s%sSTOP%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1), HIDE_CURSOR_TO_EOL);
      break;
    case USER_CMD_TR:
      Printf(server_tid, "%s%sTR %d %d%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
             cmd->data[0], cmd->data[1], HIDE_CURSOR_TO_EOL);
      break;
    case USER_CMD_SW:
      Printf(server_tid, "%s%sSW %d %c%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
             cmd->data[0], cmd->data[1], HIDE_CURSOR_TO_EOL);
      break;
    case USER_CMD_RV:
      Printf(server_tid, "%s%sRV %d%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
             cmd->data[0], HIDE_CURSOR_TO_EOL);
      break;
    case NULL_USER_CMD:
      Printf(server_tid, "%s%sINVALID COMMAND%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
             HIDE_CURSOR_TO_EOL);
      break;
    default:
      Printf(server_tid, "%s%sUNKNOWN COMMAND%s", HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
             HIDE_CURSOR_TO_EOL);
  }
}

int parse_command(char_buffer *ibuf, user_command *cmd, char data) {
  if (data == '\r') {
    if (string_starts_with(ibuf->data, "tr ")) {
      int address = parse_two_digit_number(ibuf->data + 3);
      int speed_index = address > 10 ? 6 : 5;
      int speed = parse_two_digit_number(ibuf->data + speed_index);
      cmd->type = USER_CMD_TR;
      cmd->data[0] = address;
      cmd->data[1] = speed;
    } else if (string_starts_with(ibuf->data, "rv ")) {
      int address = parse_two_digit_number(ibuf->data + 3);
      cmd->type = USER_CMD_RV;
      cmd->data[0] = address;
      cmd->data[1] = 0;
    } else if (string_starts_with(ibuf->data, "sw ")) {
      int address = parse_two_digit_number(ibuf->data + 3);
      int switch_index = address > 10 ? 6 : 5;
      int switch_num = parse_two_digit_number(ibuf->data + switch_index);
      cmd->type = USER_CMD_SW;
      cmd->data[0] = address;
      cmd->data[1] = switch_num;
    } else if (tstrcmp(ibuf->data, "stop")) {
      cmd->type = USER_CMD_STOP;
    } else if (tstrcmp(ibuf->data, "go")) {
      cmd->type = USER_CMD_GO;
    } else if (tstrcmp(ibuf->data, "q")) {
      cmd->type = USER_CMD_Q;
    }
    char_buffer_empty(ibuf);
    return true;
  } else {
    char_buffer_put(ibuf, data);
    return false;
  }
}

void k4_first_user_task() {
  EnableCaches(true);
  int my_priority = MyPriority();
#ifdef E2ETESTING
  ns_tid = Create(my_priority + 2, &nameserver_main);
  Assert(ns_tid > 0);
#else
  Assert(Create(my_priority + 2, &nameserver_main) > 0);
#endif /* E2ETESTING */
  Assert(Create(my_priority + 1, &idle_task_cursor) > 0);
  int clock_server_tid = Create(my_priority + 3, &clock_server);
  Assert(clock_server_tid > 0);

  spawn_ioservers();


  user_command current_cmd;
  char cmd_prefix[CMD_MAX_SZ];
  char_buffer current_cmd_buf;
  char_buffer_init(&current_cmd_buf, cmd_prefix, CMD_MAX_SZ);
  user_command_reset(&current_cmd);

  int terminal_tx_server = WhoIs("TerminalTxServer");
  int terminal_rx_server = WhoIs("TerminalRxServer");

  Printf(terminal_tx_server, "%s%s", RESET_TEXT, CLEAR_SCREEN);

  while (true) {
    char c = Getc(terminal_rx_server, TERMINAL);
    Assert(c >= 0);
    if (parse_command(&current_cmd_buf, &current_cmd, c)) {
      if (current_cmd.type == USER_CMD_Q) {
        break;
      } else {
        user_command_print(terminal_tx_server, &current_cmd);
        // TODO send message to user input server or whatever
      }
    }
  }

  kill_ioservers();
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
  Assert(Kill(WhoIs("Idle")) == 0);
}
