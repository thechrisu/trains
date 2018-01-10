//
// Created by Christoph Ulshöfer on 2018-01-04.
//
#include "a0terminal.h"

char track[13][23] = {
    "------12-11--------\\",
    "----4/   /          \\",
    "---/   14--13----10  |",
    "      /     \\ |  /  \\|",
    "     |    156\\|/155  9",
    "     |        |      |",
    "     |    153/|\\154  8",
    "      \\     / | \\   /|",
    "---\\   15-16----17/  |",
    "----\\1   \\          /",
    "-----\\2   ---6-----7--",
    "------3-------\\---/---",
    "              18  5"
};

void go_to_pos(int x_offset, int y_offset) {
  /*putc('\033');
  putc('[');
  if (x_offset >= 10) putc('1');
  putc('0' + (x_offset % 10));
  putc(';');
  if (y_offset >= 10) putc('1');
  putc('0' + (y_offset % 10));
  putc('H');*/
  printf("%s\033[%d;%dH", HIDE_CURSOR, x_offset, y_offset);
}

void print_track() {
  for (int i = 0; i < 13; i++) {
    go_to_pos(TRACK_X + i, TRACK_Y);
    printf("%s", track[i]);
  }
  go_to_pos(TRACK_X + 13, 0);
}

char lastSensorBuf[10];

void print_triggered_sensors(char_buffer *buf) {
  // bwputc(COM2, 'p');
  bool valChanged = false;
  for (int i = 0; i < 10; i++) {
    if (buf->data[i] != lastSensorBuf[i]) {
      valChanged = true;
    }
    }
  if (valChanged) {
    unsigned int j = 1;
    go_to_pos(SENS_X + 1, SENS_Y);
    for (unsigned int i = (buf->size + buf->in - 1) % buf->size; i != buf->in || i >= buf->in;
         i = (i - 1 + buf->size) % buf->size) {
      if (j > buf->elems) return;
      // go_to_pos(SENS_X + j++, SENS_Y);
      // putc('A' + (buf->data[i] / 16));
      // if (buf->data[i] >= 10)
      //printf("%d  ", (buf->data[i] % 16) + 1);
      //putc(';');
      //putc(' ');//(global_track_state.track[(int) buf->data[i]]).name);
      printf("%s ", global_track_state.track[(int) buf->data[i]].name);
    }
    for (int i = 0; i < 10; i++) {
      lastSensorBuf[i] = buf->data[i];
      }
    }
}

my_time last_time;

void print_time(uint32_t min, uint32_t sec, uint32_t dsec) {
  if ((last_time.min != min || last_time.sec != sec || last_time.dsec != dsec) && sec >= 10) {
    go_to_pos(TIME_X, TIME_Y);
    printf("%d:%d:%d%s", min, sec, dsec, HIDE_CURSOR_TO_EOL);
    last_time.min = min;
    last_time.sec = sec;
    last_time.dsec = dsec;
  } else {
    if (sec < 10 && dsec % 10 == 0) {
      go_to_pos(TIME_X, TIME_Y);
      printf("%s%s", HIDE_CURSOR, HIDE_CURSOR_TO_EOL);
    }
  }
}

void print_turnout(int i) {
  if (i < NUM_TURNOUTS / 2)
    go_to_pos(TURN_X + 1, TURN_Y + 7*i);
  else
    go_to_pos(TURN_X + 2, TURN_Y + 7*(i-NUM_TURNOUTS/2));
  //if (i < NUM_TURNOUTS / 2)
    // go_to_pos(TURN_X + i + 1, TURN_Y);
  //else
    // go_to_pos(TURN_X + i + 1- NUM_TURNOUTS / 2, TURN_Y + 6);
  int turnout = map_offset_to_turnout(i);
  if (turnout < 10) {
    printf("  %d:%c ", turnout, global_track_state.turnouts[i]);
  } else if (turnout < 100) {
    printf(" %d:%c ", turnout, global_track_state.turnouts[i]);
  } else {
    printf("%d:%c ", turnout, global_track_state.turnouts[i]);
  }
}

void print_turnouts() {
  go_to_pos(TURN_X, TURN_Y);
  printf("Turnouts");
  for (int i = 0; i < NUM_TURNOUTS; i++) {
    print_turnout(i);
    /*if (i < NUM_TURNOUTS / 2)
      go_to_pos(TURN_X + i + 1, TURN_Y);
    else
      go_to_pos(TURN_X + i + 1- NUM_TURNOUTS / 2, TURN_Y + 6);
    int turnout = map_offset_to_turnout(i);
    if (turnout < 10) {
      printf("  %d:%c", turnout, global_track_state.turnouts[i]);
    } else if (turnout < 100) {
      printf(" %d:%c", turnout, global_track_state.turnouts[i]);
    } else {
      printf("%d:%c", turnout, global_track_state.turnouts[i]);
      }*/
  }
}

void print_cmdline(char_buffer *termBuf) { //, str_buffer *cmdHistoryBuf) {
  /*for(uint32_t i = 0; i < cmdHistoryBuf->elems; i++) {
    go_to_pos(CMDL_X + i, CMDL_Y);
    for(uint32_t j = 0; j < cmdHistoryBuf->elem_maxsize; j++) {
      if (termBuf->data[i] == '\0') {
        break;
      } else {
        bwsendbyte_bufferedputc(COM2, cmdHistoryBuf->data[i][j]);
      }
    }
    // printf("%s", cmdHistoryBuf->data[i]);
  }*/
  go_to_pos(CMDL_X, CMDL_Y + 2);
  for (uint16_t i = 0; i < termBuf->elems; i++) {
    if (termBuf->data[i] == '\0') {
      break;
    } else {
      bwsendbyte_buffered(COM2, termBuf->data[i]);
    }
  }
  go_to_pos(CMDL_X, CMDL_Y + 2 + termBuf->elems);
  printf("%s", HIDE_CURSOR_TO_EOL);
}
