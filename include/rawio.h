/**
 * @file
 * @brief No-frills UART put/get without any checks.
 */
#ifndef RAWIO_H
#define RAWIO_H

#include "codes.h"
#include "io.h"

/**
 * Puts a character in the UART.
 * If this function gets called in the wrong spot, tough luck.
 * Better assert <code>rawcanputc()</code>.
 *
 * @param channel TRAIN/TERMINAL.
 * @param c       Character to put.
 *
 * @return -1     If channel not set to TRAIN/TERMINAL
 *          0     On success
 */
int rawputc(int channel, char c);
int rawgetc(int channel, char *c);
int rawcanputc(int channel);
int rawcangetc(int channel);
char raw_get_error(int channel);

#endif /* RAWIO_H */
