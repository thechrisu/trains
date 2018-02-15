#include "a0/a0terminal.h"
#include "a0/a0trackstate.h"
#include "track/track_node.h"
#include "track/track_data.h"

#include "assert.h"
#include "char_buffer.h"
#include "myio.h"
#include "mytimer.h"
#include "tstdlib.h"
#include "tstring.h"

#define sensorBufSize 10
#define cmdSz 10
#define cmdHistoryLength 5

char_buffer recentSensorsBuf;
char recentlyTriggeredSensors[sensorBufSize];
char rawSensors[sensorBufSize];

char_buffer termBuf; // stores prefix of current command
char cmdPrefix[cmdSz + 1]; // +1 because of the \0 hack (you'll see..)

uint64_t nloops;
uint64_t time_for_nloops = 0;
uint64_t last_loop, worst_time, last_train_send, last_train_receive;
void bootstrap() {
  init_track(&global_track_state);
  char_buffer_init(&recentSensorsBuf, recentlyTriggeredSensors, sensorBufSize);
  char_buffer_init(&termBuf, cmdPrefix, cmdSz + 1);
  setup_timer();
  setup_io();
  printf("\033[38;5;255m\033[31m%s", RESET_TEXT);
  printf("%s%s", CLEAR_SCREEN, HIDE_CURSOR);
  go_to_pos(1, 1);
  printf("\033[31m%s", RESET_TEXT);
  setup_turnouts();
  set_go(true);
  nloops = 0;
  time_for_nloops = 0;
  putc(TERMINAL, 'R');
  print_track();
  print_triggered_sensors(&recentSensorsBuf);
  print_turnouts();
  print_cmdline(&termBuf);
  go_to_pos(SENS_X, SENS_Y);
  printf("Sensors");
  go_to_pos(CMDL_X, CMDL_Y);
  sendbyte_buffered(TERMINAL, '>');
  sendbyte_buffered(TERMINAL, ' ');
}

void print_debug(char *msg) {
#if DEBUG
  go_to_pos(1, 1);
  printf("%s%s", HIDE_CURSOR_TO_EOL, msg);
#else
  (void)msg;
#endif
}

bool interpret_cmd(char_buffer *cmd_buf) {
  switch (cmd_buf->data[0]) {
    case 'g': {
      if (cmd_buf->data[1] == 'o' && cmd_buf->data[2] == '\0') {
        set_go(true);
        print_debug("GO");
      }
      return false;
    }
    case 's': {
      int nParse = is_valid_number(cmd_buf, 3);
      if (cmd_buf->data[1] == 't' && cmd_buf->data[2] == 'o'
          && cmd_buf->data[3] == 'p' && cmd_buf->data[4] == '\0') {
        set_go(false);
        print_debug("STOP");
      } else if (cmd_buf->data[1] == 'w'
                 && nParse >= 0 && cmd_buf->elems > (unsigned int) nParse + 1) {
        char change_cmd = cmd_buf->data[nParse];
        go_to_pos(2, 1);
        if ((change_cmd == 'S' || change_cmd == 'C') && cmd_buf->data[nParse + 1] == '\0') {
          int num = parse_number(cmd_buf, 3);
          if (num > 0 && (num < 19 || (num >= 153 && num <= 156))) {
            set_turnout(num, change_cmd);
#if DEBUG
            go_to_pos(1, 1);
            printf("SET TURNOUT %d TO %c%s",
                   num, change_cmd, HIDE_CURSOR_TO_EOL);
#endif
          }
        }
      }
      return false;
    }
    case 'q': {
      if (cmd_buf->data[1] == '\0') {
        putc(TRAIN, 128);
        putc(TRAIN, 0x61);
        return true;
      }
      return false;
    }
    case 't': {
      int numParse1 = is_valid_number(cmd_buf, 3);
      if (numParse1 < 0) break;
      int numParse2 = is_valid_number(cmd_buf, numParse1);
      if (cmd_buf->data[1] == 'r' && cmd_buf->data[2] == ' ' && numParse2 >= 0) {
        char loco = parse_number(cmd_buf, 3);
        char speed = parse_number(cmd_buf, numParse1);
#if DEBUG
        go_to_pos(1, 1);
        printf("SETTING TRAIN: loco %d, speed %d%s", loco, speed, HIDE_CURSOR_TO_EOL);
#endif
        set_speed(loco, speed);
        set_headlights(loco, !global_track_state.train[(int) loco].headlights);
      }
      return false;
    }
    case 'r': {
      int numParse = is_valid_number(cmd_buf, 3);
      if (numParse < 0) break;
      if (cmd_buf->data[1] == 'v' && cmd_buf->data[2] == ' ') {
        int loco = parse_number(cmd_buf, 3);
        bwreverse(loco);
#if DEBUG
        go_to_pos(1, 1);
        printf("REVERSING %d%s", loco, HIDE_CURSOR_TO_EOL);
        go_to_pos(3, 1);
        printf("%s", HIDE_CURSOR_TO_EOL);
#endif
      }
      return false;
    }
    default:
      return false;
  }
  return false; // appease gcc
}
void a0_main() {
  bootstrap();
  my_time t;
  bool shouldStop = false;
  worst_time = 0;
  last_loop = 0;
  nloops = 0;
  uint32_t timestamp;

  while (!shouldStop) {
    last_loop = get_clockticks();
    time_for_nloops = get_clockticks();
    nloops++;
    get_time_struct(&t, &timestamp);
    print_time(t.min, t.sec, t.dsec);
    trysendbyte(TERMINAL);
    get_time_struct(&t, &timestamp);
    trysendbyte(TERMINAL);
    check_reverse(timestamp);
    trysendbyte(TERMINAL);
    check_sensors(rawSensors, &recentSensorsBuf, timestamp);
    turn_off_solenoid_if_necessary(timestamp);
    trysendbyte(TERMINAL);
    check_turnout_buffer();
    trysendbyte(TERMINAL);
    tryreceiveall();
    trysendall();
    if (canreadbyte_buffered(TERMINAL)) {
      char c = readbyte_buffered(TERMINAL);
      if (c == 13) { // newline
        char_buffer_put(&termBuf, '\0');
        shouldStop = interpret_cmd(&termBuf);
        char_buffer_empty(&termBuf);
      } else {
        char_buffer_put(&termBuf, c);
      }
      print_cmdline(&termBuf);
      go_to_pos(CMDL_X, CMDL_Y + 2 + termBuf.elems);
      printf("%s", SHOW_CURSOR);
    }
#if DEBUG
    if (nloops < 1000) continue;
    uint64_t this_loop = get_clockticks() - last_loop;
    worst_time = this_loop > worst_time ? this_loop : worst_time;
    if (nloops % 1000 == 0) {
      go_to_pos(1, 1);
      printf("%u, %u%s", (uint16_t) this_loop, worst_time, HIDE_CURSOR_TO_EOL); //((10000 * 100 * time_for_nloops) / (nloops * 5084689)), HIDE_CURSOR_TO_EOL);
    }
#endif
  }
}
