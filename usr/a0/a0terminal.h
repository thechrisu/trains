/**
 * @file
 * @brief Functions that print the state of the track to the terminal
 */
#ifndef A0_TERMINAL_H
#define A0_TERMINAL_H

extern struct track_state global_track_state;

#include "char_buffer.h"
#include "myio.h"
#include "mytimer.h"
#include "../track/track_data.h"
#include "../track/track_node.h"

#define RESET_TEXT "\033[0m"
#define BLACK_TEXT "\033[30m"
#define RED_TEXT "\033[31m"
#define GREEN_TEXT "\033[32m"
#define YELLOW_TEXT "\033[33m"
#define BLUE_TEXT "\033[34m"
#define MAGENTA_TEXT "\033[35m"
#define CYAN_TEXT "\033[36m"
#define WHITE_TEXT "\033[37m"

#define CLEAR_SCREEN "\033[2J"
#define CURSOR_UPPER_LEFT "\033[H"
#define CURSOR_ROW_COL(r, c) ("\033[" #r ";" #c "H")
#define HIDE_CURSOR "\033[?25h"
#define SHOW_CURSOR "\033[?25l"
#define HIDE_CURSOR_TO_EOL "\033[K"

#if DEBUG
#define TOP_SPACE 10
#else
#define TOP_SPACE 0
#endif

#define TRACK_X 2 + TOP_SPACE
#define TRACK_Y 1
#define SENS_X 2 + TOP_SPACE
#define SENS_Y 27
#define TURN_X 2 + TOP_SPACE
#define TURN_Y 35
#define TIME_X 1 + TOP_SPACE
#define TIME_Y 1
#define CMDL_X 14 + TOP_SPACE
#define CMDL_Y 1

void go_to_pos(int x_offset, int y_offset);
void print_track();
void print_time(uint32_t min, uint32_t sec, uint32_t dsec);
void print_turnouts();
void print_turnout(int i);
void print_triggered_sensors(char_buffer *t_sens_buf);
void print_cmdline(char_buffer *termBuf); //, str_buffer *cmdHistoryBuf);

#endif /* A0_TERMINAL_H */
