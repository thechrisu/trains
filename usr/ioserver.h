/**
 * @file
 * @brief I/O Server implementations.
 */
#ifndef IOSERVER_H
#define IOSERVER_H

#include "tstdlib.h"
#include "codes.h"
#include "char_buffer.h"
#include "int32_t_buffer.h"
#include "notifier.h"
#include "rawio.h"

void train_tx_server();
void train_rx_server();
void terminal_tx_server();
void terminal_rx_server();
void spawn_ioservers();

#endif /* IOSERVER_H */
