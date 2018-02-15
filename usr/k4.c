#include "k4.h"

#define CMD_MAX_SZ 32

void char_buffer_clear(char_buffer *b) {
  for (unsigned int i = 0; i < b->elems; i++) {
    b->data[i] = 0;
  }
}

/**
 * To print a user command to the terminal.
 *
 * @param server_tid The task id of the Terminal transmit server.
 * @param cmd        The command to send.
 */
void user_command_print(int server_tid, user_command *cmd) {
  switch(cmd->type) {
    case USER_CMD_GO:
      Assert(Printf(server_tid, "%s%s%s%sGO        %s%s",
                    GREEN_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR,
                    CURSOR_ROW_COL(CMD_LINE, 1), HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
      break;
    case USER_CMD_STOP:
      Assert(Printf(server_tid, "%s%s%s%sSTOP       %s%s",
                    GREEN_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1), HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
      break;
    case USER_CMD_TR:
      Assert(Printf(server_tid, "%s%s%s%sTR %d %d     %s%s",
                    GREEN_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
                    cmd->data[0], cmd->data[1], HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
      break;
    case USER_CMD_SW:
      Assert(Printf(server_tid, "%s%s%s%sSW %d %c         %s%s",
                    GREEN_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
                    cmd->data[0], cmd->data[1], HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
      break;
    case USER_CMD_RV:
      Assert(Printf(server_tid, "%s%s%s%sRV %d           %s%s",
                    GREEN_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
                    cmd->data[0], HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
      break;
    case NULL_USER_CMD:
      Assert(Printf(server_tid, "%s%s%s%sINVALID COMMAND        %s%s",
                    RED_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
                    HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
      break;
    default:
      Assert(Printf(server_tid, "%s%s%s%sUNKNOWN COMMAND          %s%s",
                    RED_TEXT, HIDE_CURSOR_TO_EOL, HIDE_CURSOR, CURSOR_ROW_COL(CMD_LINE, 1),
                    HIDE_CURSOR_TO_EOL, RESET_TEXT) == 0);
  }
}

/**
 * @param data    The latest character received.
 * @param cmd     User command, the real return value.
 * @param ibuf    The buffer holding the input string.
 * @return        1 iff it parsed a command.
 */
int parse_command(char_buffer *ibuf, user_command *cmd, char data) { // I apologize for this mess
  if (data == '\r') {
    if (string_starts_with(ibuf->data, "tr ", ibuf->elems)) {
      int first_num_parse = is_valid_number(ibuf, 3);
      if (first_num_parse >= 0) {
        int second_num_parse = is_valid_number(ibuf, first_num_parse);
        if (second_num_parse >= 0 && ibuf->elems >= (unsigned int) second_num_parse) {
          int address = parse_two_digit_number(ibuf->data + 3);
          int speed = parse_two_digit_number(ibuf->data + first_num_parse);
          if (speed >= 0 && speed <= 14) {
            cmd->type = USER_CMD_TR;
            cmd->data[0] = address;
            cmd->data[1] = speed;
          }
        }
      }
    } else if (string_starts_with(ibuf->data, "rv ", ibuf->elems)) {
      int nParse = is_valid_number(ibuf, 3);
      if (nParse >= 0 && ibuf->elems >= (unsigned int) nParse) {
        int address = parse_two_digit_number(ibuf->data + 3);
        cmd->type = USER_CMD_RV;
        cmd->data[0] = address;
        cmd->data[1] = 0;
      }
    } else if (string_starts_with(ibuf->data, "sw ", ibuf->elems)) {
      int nParse = is_valid_number(ibuf, 3);
      if (nParse >= 0 && ibuf->elems == (unsigned int) nParse + 1) {
        char change_cmd = ibuf->data[nParse];
        if (change_cmd == 'S' || change_cmd == 'C') {
          int num = parse_number(ibuf, 3);
          if (num > 0 && (num < 19 || (num >= 153 && num <= 156))) {
            cmd->type = USER_CMD_SW;
            cmd->data[0] = num;
            cmd->data[1] = change_cmd;
          }
        }
      }
    } else if (string_starts_with(ibuf->data, "stop", ibuf->elems) && ibuf->elems == 4) {
      cmd->type = USER_CMD_STOP;
    } else if (string_starts_with(ibuf->data, "go", ibuf->elems) && ibuf->elems == 2) {
      cmd->type = USER_CMD_GO;
    } else if (string_starts_with(ibuf->data, "q", ibuf->elems) && ibuf->elems == 1) {
      cmd->type = USER_CMD_Q;
    }
    char_buffer_clear(ibuf);
    char_buffer_empty(ibuf);
    return true;
  } else {
    char_buffer_put(ibuf, data);
    return false;
  }
}

#define K_LINE 12

void delete_from_char(int index, int recipient) {
  Assert(Printf(recipient, "%s%d;%dH%s", ESC, K_LINE, 3 + index, HIDE_CURSOR_TO_EOL) == 0);
}

void print_cmd_char(char c, int index, int recipient) {
  Assert(Printf(recipient, "%s%d;%dH%c%s%s", ESC, K_LINE, 3 + index, c, HIDE_CURSOR_TO_EOL, HIDE_CURSOR) == 0);
}

#define max(a, b) (a > b ? a : b)

void k4_first_user_task() {
  EnableCaches(true);

  int my_priority = MyPriority();
#ifdef E2ETESTING
  ns_tid = Create(my_priority + 2, &nameserver_main);
  Assert(ns_tid > 0);
#else
  Assert(Create(my_priority + 2, &nameserver_main) > 0);
#endif /* E2ETESTING */
  int clock_server_tid = Create(my_priority + 3, &clock_server);
  Assert(clock_server_tid > 0);
  int idle_tid = Create(my_priority - 1, &idle_task_cursor);
  Assert(idle_tid > 0);

  spawn_ioservers();
  int cmd_dispatcher_tid = Create(my_priority, &command_dispatcher_server);
  Assert(cmd_dispatcher_tid > 0);
  Assert(Create(my_priority + 7, &track_state_controller) > 0);
  Assert(Create(my_priority + 4, &sensor_secretary) > 0);

  message cmd_msg;
  cmd_msg.type = MESSAGE_USER;

  user_command current_cmd;
  char cmd_prefix[CMD_MAX_SZ];
  char_buffer current_cmd_buf;
  char_buffer_init(&current_cmd_buf, cmd_prefix, CMD_MAX_SZ);
  user_command_reset(&current_cmd);

  int terminal_tx_server = WhoIs("TerminalTxServer");
  int terminal_rx_server = WhoIs("TerminalRxServer");

#ifndef E2ETESTING
  Assert(Printf(terminal_tx_server, "%s%s", RESET_TEXT, CLEAR_SCREEN) == 0);
  Assert(Printf(terminal_tx_server, "%s%d;%dH%c%s", ESC, K_LINE, 1, '>', HIDE_CURSOR_TO_EOL) == 0);

  cmd_msg.msg.cmd.type = USER_CMD_GO;
  Assert(Send(cmd_dispatcher_tid, &cmd_msg, sizeof(cmd_msg), EMPTY_MESSAGE, 0) == 0);

  Assert(Create(my_priority - 1, &switch_resetter) > 0);

  Assert(Create(my_priority + 1, &clock_view) > 0);
#endif /* E2ETESTING */

  while (true) {
    int c = Getc(terminal_rx_server, TERMINAL);
    Assert(c >= 0);
    if (c == 127 || c == 8) { // backspace
      current_cmd_buf.in = current_cmd_buf.in -1;
      current_cmd_buf.data[current_cmd_buf.in] = 0;
      current_cmd_buf.elems = current_cmd_buf.elems - 1;
      delete_from_char(current_cmd_buf.in + 1, terminal_tx_server);
      continue;
    }
    if (parse_command(&current_cmd_buf, &current_cmd, c)) {
      if (current_cmd.type != NULL_USER_CMD) {
        cmd_msg.msg.cmd.type = current_cmd.type;
        cmd_msg.msg.cmd.data[0] = current_cmd.data[0];
        cmd_msg.msg.cmd.data[1] = current_cmd.data[1];
        Assert(Send(cmd_dispatcher_tid, &cmd_msg, sizeof(cmd_msg), EMPTY_MESSAGE, 0) == 0);
      }

      if (current_cmd.type == USER_CMD_Q) {
        break;
      } else {
        user_command_print(terminal_tx_server, &current_cmd);
        delete_from_char(0, terminal_tx_server);
      }
    } else {
      print_cmd_char(c, current_cmd_buf.in, terminal_tx_server);
    }
  }
  Assert(Printf(terminal_tx_server, "%sBye.\n\r\n\r", CURSOR_ROW_COL(K_LINE, 1)) == 0);
  kill_ioservers();
  Assert(Kill(WhoIs("CommandDispatcher")) == 0);
  Assert(Kill(WhoIs("ClockNotifier")) == 0);
#ifdef E2ETESTING
  Assert(Kill(ns_tid) == 0);
#endif /* E2ETESTING */
  Assert(Kill(idle_tid) == 0);
}
