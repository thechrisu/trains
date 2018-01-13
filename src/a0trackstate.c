//turn
// Created by Christoph Ulshöfer on 2018-01-04.
//
#include "a0trackstate.h"

enum switch_state {
  STRAIGHT,
  CURVED
};

void blink_headlights(char loco) {
  sendbyte_buffered(TRAIN, 16);
  sendbyte_buffered(TRAIN, loco);
  sendbyte_buffered(TRAIN, 0);
  sendbyte_buffered(TRAIN, loco);
}

char_buffer turnout_buffer;
char turnouts_to_set[200];

void setup_turnouts() {
  char_buffer_init(&turnout_buffer, turnouts_to_set, 200);
  for (int i = 0; i < NUM_TURNOUTS; i++) {
    if (map_offset_to_turnout(i) == 153 || map_offset_to_turnout(i) == 155) continue;
    set_turnout(map_offset_to_turnout(i), 'C');
  }
  sendbyte_buffered(TRAIN, 192);
}
int calls_to_turnout_buffer = 0;
void check_turnout_buffer() {
  if (char_buffer_get_num_elems(&turnout_buffer) >= 2 && !global_track_state.should_switch_off_solenoid && global_track_state.last_switch_time + 150 < get_time()) {
    char turnout, pos;
    pos = char_buffer_get(&turnout_buffer);
    turnout = char_buffer_get(&turnout_buffer);
    global_track_state.turnouts[turnout_num_to_map_offset(turnout)] = ((pos == 33) ? 'S' : 'C');
    sendbyte_buffered(TRAIN, pos);
    sendbyte_buffered(TRAIN, turnout);
    print_turnout(turnout_num_to_map_offset(turnout));
#if DEBUG
    printf("SETTING TURNOUT %d, %c%s", turnout, (pos == 33) ? 'S' : 'C', HIDE_CURSOR_TO_EOL);
    go_to_pos(3, 1);
    printf("%s", HIDE_CURSOR_TO_EOL);
#endif
    global_track_state.last_switch_time = get_time();
    global_track_state.should_switch_off_solenoid = true;
  }
}

/**
 *
 * /byte 1:
 * 0x21 straight
 * 0x22 curved
 * byte 2: switch address 0-255
 *
 * 0-15 speed, train address
 * 192/C0 + sensor number (1-31) for sensors
 * 128/80 + sensor number (1-31) parallel
 * 0x61 STOP, 0x60 GO
 * @param speed_func speed 0-14, reverse: speed 15
 * @param loco 1-80
 */
void send_train_signal(char speed, char loco, bool headlights_on) {
  char b1, b2;
  b1 = speed + (headlights_on ? 16 : 0);
  b2 = loco;
  sendbyte_buffered(TRAIN, b1);
  sendbyte_buffered(TRAIN, b2);
}

void send_turnout_signal(char turnout, bool shouldStraight) {
  assert(turnout > 0 && turnout <= (char) 156 && !(turnout >= 19 && turnout < (char) 153));
  char_buffer_put(&turnout_buffer, 33 + (shouldStraight ? 0 : 1));
  char_buffer_put(&turnout_buffer, turnout);
}

void set_go(bool go) {
  if (go) {
    sendbyte_buffered(TRAIN, 0x60);
  } else {
    sendbyte_buffered(TRAIN, 0x61);
  }
}

void set_speed(int train, int speed) {
  // go_to_pos(3, 1);
  // printf("speed: %d, train: %d", speed, train);
  assert(speed <= 15);
  assert(train <= 80 && train > 0);
  global_track_state.train[train].should_speed = speed;
  send_train_signal(speed, train, global_track_state.train[train].headlights);
}

void set_headlights(int train, bool turn_on) {
  assert(train <= 80 && train > 0);
  global_track_state.train[train].headlights = turn_on;
  send_train_signal(global_track_state.train[train].should_speed, train, turn_on);
}

/*void reverse_train(int train) {
  int goal_speed_abs = global_track_state.train.should_speed;
  global_track_state.train.direction = !global_track_state.train.direction;
  global_track_state.train.should_speed = 0;
  set_loco(15, train);
  global_track_state.train.should_speed = goal_speed_abs;
  set_loco(goal_speed_abs, train);
}*/

void turn_off_solenoid_if_necessary(uint32_t timestamp) {
  if (global_track_state.should_switch_off_solenoid) {
    if (global_track_state.last_switch_time + 150 < timestamp) {
      global_track_state.should_switch_off_solenoid = false;
      sendbyte_buffered(TRAIN, 32);
#if DEBUG
      go_to_pos(3, 1);
      printf("TURNING OFF SOLENOID (%d, %d)", global_track_state.last_switch_time, (uint32_t) timestamp);
#endif
      global_track_state.last_switch_time = timestamp;
    }
  }
}

void reverse(int train) {
  // set_speed(train, 0);
  if (global_track_state.train[train].should_restart || global_track_state.train[train].should_restart) return;
  uint16_t should_speed = global_track_state.train[train].should_speed;
  if (should_speed == 0) {
    set_speed(train, 15);
    global_track_state.train[train].should_speed = should_speed;
    go_to_pos(2, 1);
    // printf("RV DIRECTLY %d", train);
    return;
  }
  set_speed(train, 0);
  set_speed(train, 0);
  char_buffer_put(&(global_track_state.trains_to_reverse), train);
  global_track_state.train[train].should_restart = true;
  global_track_state.train[train].should_speed = should_speed;
  global_track_state.train[train].time_reverse_sent = get_time();
}

void check_reverse(uint32_t timestamp) {
  (void)timestamp;
#if DEBUG
  if (last_print < get_time()) {
    go_to_pos(1, 1);
    for(unsigned int j = 0; j < global_track_state.trains_to_reverse.elems; j++) {
      printf("%d, ", global_track_state.trains_to_reverse.data[(j + global_track_state.trains_to_reverse.out) % global_track_state.trains_to_reverse.size]);
    }
    printf("%s", HIDE_CURSOR_TO_EOL);
    // printf("%d, %d, %d%s", pos, train, global_track_state.trains_to_reverse.elems, HIDE_CURSOR_TO_EOL);
    last_print = get_time() + 500;
  }
#endif
  for(unsigned int i = 0; i < global_track_state.trains_to_reverse.elems; i++) {
    unsigned int pos = (i + global_track_state.trains_to_reverse.out) % global_track_state.trains_to_reverse.size;
    unsigned int train = global_track_state.trains_to_reverse.data[pos];
    if (global_track_state.train[train].should_restart && global_track_state.train[train].time_reverse_sent + 4000 < get_time()) {
      //go_to_pos(3, 1);
      //printf("REVERSING %d)", train);
      global_track_state.train[train].sent_reverse = true;
      global_track_state.train[train].should_restart = false;
      uint16_t should_speed = global_track_state.train[train].should_speed;
      set_speed(train, 15);
      global_track_state.train[train].should_speed = should_speed;
      global_track_state.train[train].time_reverse_sent = get_time();
      continue;//return;
    }
    if (global_track_state.train[train].sent_reverse && global_track_state.train[train].time_reverse_sent + 1000 < get_time()) {
      global_track_state.train[train].sent_reverse = false;
      set_speed(train, global_track_state.train[train].should_speed);
      global_track_state.train[train].direction = !global_track_state.train[train].direction;
      //go_to_pos(2, 1);
      //printf("SWITCHING DIR FOR TRAIN %d (speed: %d)", train, global_track_state.train[train].should_speed);
      global_track_state.train[train].should_restart = false;
      char_buffer_get(&(global_track_state.trains_to_reverse));//, pos); //global_track_state.train[train].trains_to_reverse = 0;
      global_track_state.train[train].time_reverse_sent = 0;
    }
  }
}

uint32_t last_sens = 0;
uint8_t sensorBytesRead = 0;

void query_sensors() {
  sendbyte_buffered(TRAIN, 128 + 5);
  // go_to_pos(4, 1);
  last_sens = get_time();
  sensorBytesRead = 0;
  oe_in_sensor = 0;
}

// TODO make things either consistenly snake_case or PascalCase
void check_sensors(char *sensor_state, char_buffer *recentSensorsBuf, uint32_t timestamp) {
  (void) sensor_state;
  if (global_track_state.last_sensor_query + (uint32_t) 100 < timestamp) {
    // printf("%d, %d\n", global_track_state.last_sensor_query, timestamp);
    if (timestamp < 10000) {
      char_buffer_empty(recentSensorsBuf);
    } else {
      print_triggered_sensors(recentSensorsBuf);
    }
    query_sensors();
    global_track_state.last_sensor_query = timestamp;
    return;
  }
  while (getnumreadable_bytes(TRAIN) >= 2 && !oe_in_sensor) {
    sensorBytesRead += 2;
    uint16_t s88mod = (readbyte_buffered(TRAIN) << 8) | readbyte_buffered(TRAIN);
    for (int8_t i = 15; i >= 0; i--) {
      uint16_t msk = 1 << (16 - i - 1); // first: 9-16, second: 1-8
      if (s88mod & msk) {
        char ltr_num = sensorBytesRead / 2 - 1;
        char sensor = ltr_num * 16 + i;
        if (recentSensorsBuf->elems > 0
            && recentSensorsBuf->data[(recentSensorsBuf->elems + recentSensorsBuf->in - 1) % recentSensorsBuf->elems]
                == sensor)
          continue;
        // go_to_pos(1, 1);
        // printf("%d, %d, %d                   %d        ", sensor, i, sensorBytesRead, s88mod);
        if (char_buffer_is_full(recentSensorsBuf)) {
          char_buffer_get(recentSensorsBuf);
        }
        char_buffer_put(recentSensorsBuf, sensor);
      }
    }
    if (sensorBytesRead == 10) {
      empty_buf(TRAIN, false); // input
    }
  }
  if (sensorBytesRead == 10 || oe_in_sensor) { // 5 sensor modules!
    sensorBytesRead = 0;
    empty_buf(TRAIN, false);
    if (!oe_in_sensor) {
#if DEBUG
      go_to_pos(4, 1);
      printf("%ums%s", (uint16_t)(get_time() - last_sens), HIDE_CURSOR_TO_EOL);
#endif
      go_to_pos(6, 1);
      print_triggered_sensors(recentSensorsBuf);
      query_sensors();
      global_track_state.last_sensor_query = timestamp;
    }
  }
}

void set_turnout(int turnout_num, char state) {
#if DEBUG
  if ((turnout_num >= 19 && turnout_num <= 152) || turnout_num > 156) {
    go_to_pos(1, 1);
    printf("Illegal turnout num %d%s", turnout_num, HIDE_CURSOR_TO_EOL);
  }
#endif
  assert(state == 'C' || state == 'S');
  bool shouldStraight = state == 'S';
  // int current = turnout_num_to_map_offset(turnout_num);
  // if (global_track_state.turnouts[current] == state) return;
  if (turnout_num == 156 || turnout_num == 154) {
    send_turnout_signal(turnout_num - 1, !shouldStraight);
  } else if (turnout_num == 155 || turnout_num == 153) {
    send_turnout_signal(turnout_num + 1, !shouldStraight);
  }
  send_turnout_signal(turnout_num, shouldStraight);
}
