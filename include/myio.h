/**
 * @file
 * @brief I/O (wrapper) functions
 */
#ifndef MY_IO_H
#define MY_IO_H

#include "bwio.h"
#include "iio.h"

#define LOG_BUFFER_SIZE 1<<22

extern char log_buffer[LOG_BUFFER_SIZE];
extern uint32_t log_index;

/**
 * Input: TO the ARM box
 * Output: FROM the ARM box
 */

void setup_io();
void desetup_iio();
void setup_iio();
int setfifo(int channel, int state);
int setspeed(int channel, int speed);

void printf(char *format, ...) __attribute((format(printf, 1, 0)));
void bwprintf(char *format, ...) __attribute((format(printf, 1, 0)));
void logprintf(char *format, ...) __attribute((format(printf, 1, 0)));
int logputc(char c);
int logputr(unsigned int reg);
void dump_logs();

#endif /* MY_IO_H */
