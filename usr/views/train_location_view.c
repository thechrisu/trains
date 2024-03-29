#include "train_location_view.h"

#define ABS(x) ((x) < 0 ? -(x) : (x))

#define SENSOR_MAYBE(node) ((node) == NULL_TRACK_NODE ? NO_NEXT_SENSOR : (node)->num)

#define MAX_TRAINS 4

#define LINE_CLEARED  1357

#define HEADER        "Train | Dest | Next |    Time | Diff (s) | Diff (cm)"

#define TRAIN_COL     0
#define DEST_COL      1
#define NEXT_COL      2
#define TIME_COL      3
#define TIME_DIFF_COL 4
#define DIST_DIFF_COL 5

static int columns[] = { 1, 9, 16, 23, 33, 44 };
static int column_widths[] = { 5, 4, 4, 7, 8, 9 };

void clear_cell(int terminal_tx_server_tid, int line, int column) {
  char output_buffer[50];

  char start_string[] = "\033[%d;%dH";
  int start_string_length = tstrlen(start_string);
  tmemcpy(output_buffer, start_string, start_string_length * sizeof(char));

  for (int i = 0; i < column_widths[column]; i += 1) {
    output_buffer[start_string_length + i] = ' ';
  }

  int end_index = start_string_length + column_widths[column];

  if (column == DIST_DIFF_COL) {
    output_buffer[end_index] = '\0';
  } else {
    output_buffer[end_index] = ' ';
    output_buffer[end_index + 1] = '|';
    output_buffer[end_index + 2] = '\0';
  }

  Assert(Printf(terminal_tx_server_tid, output_buffer,
                TRAIN_LOCATION_LINE + 2 + line, columns[column]) == 0);
}

void print_destination(int terminal_tx_server_tid, int line, unsigned int destination) {
  if (destination == NO_DESTINATION) {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH    ",
                  TRAIN_LOCATION_LINE + 2 + line, columns[DEST_COL]) == 0);
  } else {
    Assert(Printf(terminal_tx_server_tid, "\033[%d;%dH %s%c%d",
                  TRAIN_LOCATION_LINE + 2 + line, columns[DEST_COL],
                  sensor_index(destination) < 10 ? " " : "",
                  sensor_bank(destination), sensor_index(destination)) == 0);
  }
}

void print_next_sensor_prediction(int terminal_tx_server_tid, int line,
                                  unsigned int next_sensor,
                                  int expected_ticks_next_sensor_hit) {
  if (next_sensor == NO_NEXT_SENSOR) {
    clear_cell(terminal_tx_server_tid, line, NEXT_COL);
    clear_cell(terminal_tx_server_tid, line, TIME_COL);
  } else {
    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dH %s%c%d",
                  TRAIN_LOCATION_LINE + 2 + line, columns[NEXT_COL],
                  sensor_index(next_sensor) < 10 ? " " : "",
                  sensor_bank(next_sensor), sensor_index(next_sensor)) == 0);

    if (expected_ticks_next_sensor_hit == INFINITE_TICKS) {
      clear_cell(terminal_tx_server_tid, line, TIME_COL);
    } else {
      uint32_t minutes = expected_ticks_next_sensor_hit / 100 / 60;
      uint32_t seconds = (expected_ticks_next_sensor_hit / 100) % 60;
      uint32_t deciseconds = (expected_ticks_next_sensor_hit / 10) % 10;

      Assert(Printf(terminal_tx_server_tid,
                    "\033[%d;%dH%s%d:%s%d.%d",
                    TRAIN_LOCATION_LINE + 2 + line, columns[TIME_COL],
                    minutes < 10 ? " " : "",
                    minutes, seconds < 10 ? "0" : "", seconds, deciseconds) == 0);
    }
  }
}

void print_diffs(int terminal_tx_server_tid, int line,
                 unsigned int expected_last_sensor, unsigned int last_sensor,
                 int expected_ticks_last_sensor_hit, int ticks_last_sensor_hit,
                 int distance_diff) {
  if (expected_last_sensor == NO_NEXT_SENSOR ||
      expected_last_sensor != last_sensor ||
      expected_ticks_last_sensor_hit == INFINITE_TICKS) {
    clear_cell(terminal_tx_server_tid, line, TIME_DIFF_COL);
    clear_cell(terminal_tx_server_tid, line, DIST_DIFF_COL);
  } else {
    int ticks_diff = ticks_last_sensor_hit - expected_ticks_last_sensor_hit;
    int seconds = ABS(ticks_diff) / 100;
    int deciseconds = ABS(ticks_diff / 10) % 10;
    int centiseconds = ABS(ticks_diff) % 10;

    int centimetres = ABS(distance_diff / 100) / 10;
    int millimetres = ABS(distance_diff / 100) % 10;

    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dH  %s%c%d.%d%d",
                  TRAIN_LOCATION_LINE + 2 + line, columns[TIME_DIFF_COL],
                  seconds < 10 ? " " : "",
                  ticks_diff < 0 ? '-' : ' ',
                  seconds, deciseconds, centiseconds) == 0);

    Assert(Printf(terminal_tx_server_tid,
                  "\033[%d;%dH    %s%c%d.%d",
                  TRAIN_LOCATION_LINE + 2 + line, columns[DIST_DIFF_COL],
                  centimetres < 10 ? " " : "",
                  distance_diff < 0 ? '-' : ' ',
                  centimetres, millimetres) == 0);
  }
}

static void clear_line(int terminal_tx_server_tid, int line) {
  clear_cell(terminal_tx_server_tid, line, TRAIN_COL);
  clear_cell(terminal_tx_server_tid, line, DEST_COL);
  clear_cell(terminal_tx_server_tid, line, NEXT_COL);
  clear_cell(terminal_tx_server_tid, line, TIME_COL);
  clear_cell(terminal_tx_server_tid, line, TIME_DIFF_COL);
  clear_cell(terminal_tx_server_tid, line, DIST_DIFF_COL);
}

void train_location_view() {
  int clock_server_tid = WhoIs("ClockServer");
  Assert(clock_server_tid > 0);

  int track_state_controller_tid = WhoIs("TrackStateController");
  Assert(track_state_controller_tid > 0);

  int terminal_tx_server_tid = WhoIs("TerminalTxServer");
  Assert(terminal_tx_server_tid > 0);

  int sensor_interpreter_tid = WhoIs("SensorInterpreter");
  Assert(sensor_interpreter_tid > 0);

  int train_coordinates_server_tid = WhoIs("TrainCoordinatesServer");
  Assert(train_coordinates_server_tid > 0);

  int command_dispatcher_tid = WhoIs("CommandDispatcher");
  Assert(command_dispatcher_tid > 0);

  Assert(Printf(terminal_tx_server_tid, "\033[%d;%dHSensor Prediction Spot%s:%s",
                TRAIN_LOCATION_LINE, 1, TRADEMARK, HIDE_CURSOR_TO_EOL) == 0);
  Assert(Printf(terminal_tx_server_tid,
                "\033[%d;%dH%s%s",
                TRAIN_LOCATION_LINE + 1, 1,
                HEADER, HIDE_CURSOR_TO_EOL) == 0);

  int line_states[MAX_TRAINS];
  for (int i = 0; i < MAX_TRAINS; i += 1) {
    clear_line(terminal_tx_server_tid, i);
    line_states[i] = LINE_CLEARED;
  }

  turnout_state turnout_states[NUM_TURNOUTS];
  get_turnouts(track_state_controller_tid, turnout_states);

  coordinates current_prediction[81];
  coordinates current[81];
  reply_get_last_sensor_hit last_sensor[81];
  unsigned int destinations[81];

  for (int train = 1; train <= 80; train += 1) {
    get_last_sensor_hit(sensor_interpreter_tid, train, &last_sensor[train]);
    get_coordinates(train_coordinates_server_tid, train, &current[train]);
    predict_sensor_hit(train_coordinates_server_tid,
                       turnout_states,
                       train, &current_prediction[train]);
    destinations[train] = get_train_destination(command_dispatcher_tid, train);
  }

  int loops = 0;

  while (true) {
    for (int i = 0; i < num_active_trains && i < MAX_TRAINS; i += 1) {
      int train = active_trains[i];

      reply_get_last_sensor_hit seen_sensor;
      get_last_sensor_hit(sensor_interpreter_tid, train, &seen_sensor);

      if (line_states[i] != train && i < MAX_TRAINS) {
        clear_line(terminal_tx_server_tid, i);
        Assert(Printf(terminal_tx_server_tid,
                      "\033[%d;%dH   %s%d",
                      TRAIN_LOCATION_LINE + 2 + i, columns[TRAIN_COL],
                      train < 10 ? " " : "", train) == 0);
        line_states[i] = train;
      }

      unsigned int destination = get_train_destination(command_dispatcher_tid, train);
      if (destination != destinations[train]) {
        print_destination(terminal_tx_server_tid, i, destination);
        destinations[train] = destination;
      }

      if (seen_sensor.sensor != last_sensor[train].sensor) {
        get_turnouts(track_state_controller_tid, turnout_states);

        print_diffs(terminal_tx_server_tid, i,
                    current_prediction[train].loc.node->num, seen_sensor.sensor,
                    current_prediction[train].ticks, seen_sensor.ticks,
                    distance_diff(&track, turnout_states,
                                  seen_sensor.sensor, &current[train].loc));

        predict_sensor_hit(train_coordinates_server_tid,
                           turnout_states,
                           train, &current_prediction[train]);

        print_next_sensor_prediction(terminal_tx_server_tid, i,
                                     SENSOR_MAYBE(current_prediction[train].loc.node),
                                     current_prediction[train].ticks);
      } else if (seen_sensor.sensor != NO_DATA_RECEIVED) {
        get_turnouts(track_state_controller_tid, turnout_states);
        coordinates tentative_prediction;
        predict_sensor_hit(train_coordinates_server_tid,
                           turnout_states,
                           train, &tentative_prediction);

        if (tentative_prediction.loc.node == current_prediction[train].loc.node) {
          if (tentative_prediction.ticks / 10 != current_prediction[train].ticks / 10) {
            print_next_sensor_prediction(terminal_tx_server_tid, i,
                                         SENSOR_MAYBE(tentative_prediction.loc.node),
                                         tentative_prediction.ticks);
          }

          tmemcpy(&current_prediction[train],
                  &tentative_prediction,
                  sizeof(current_prediction[train]));
        } else {
          print_next_sensor_prediction(terminal_tx_server_tid, i,
                                       SENSOR_MAYBE(tentative_prediction.loc.node),
                                       tentative_prediction.ticks);
        }
      }

      tmemcpy(&last_sensor[train], &seen_sensor, sizeof(last_sensor[train]));

      get_coordinates(train_coordinates_server_tid, train, &current[train]);
    }

    for (int i = num_active_trains; i < MAX_TRAINS; i += 1) {
      if (line_states[i] != LINE_CLEARED) {
        clear_line(terminal_tx_server_tid, i);
        line_states[i] = LINE_CLEARED;
      }
    }

    loops += 1;
    DelayUntil(clock_server_tid, REFRESH_PERIOD * loops);
  }
}
