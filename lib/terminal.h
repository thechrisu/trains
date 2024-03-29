/**
 * @file
 * @brief Terminal control sequences.
 */
#ifndef TERMINAL_H
#define TERMINAL_H

#define REFRESH_PERIOD 6

#define IDLE_LINE             1
#define CLOCK_LINE            2
#define SENSOR_HEADING_LINE   4
#define TURNOUTS_HEADING_LINE 4
#define TURNOUTS_COLUMN       12
#define CALIB_LINE            19
#define TRAIN_LOCATION_LINE   23
#define RESERVATION_LINE      30
#define GROUP_LINE            37
#define CMD_LINE              44
#define PROMPT_LINE           45

#define RESET_TEXT "\033[0m"
#define BOLD_TEXT "\033[1m"
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
#define ESC ("\033[")
#define ESC_CH ('\033')
#define __CURSOR_ROW_COL(r, c) ("\033[" #r ";" #c "H")
#define CURSOR_ROW_COL(r, c) __CURSOR_ROW_COL(r, c)
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"
#define HIDE_CURSOR_TO_EOL "\033[K"

#define TRADEMARK "\xE2\x84\xA2"

#endif /* TERMINAL_H */
