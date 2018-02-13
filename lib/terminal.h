/**
 * @file
 * @brief Terminal control sequences.
 */
#ifndef TERMINAL_H
#define TERMINAL_H

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

#endif /* TERMINAL_H */
