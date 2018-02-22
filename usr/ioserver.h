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
#include "terminal.h"

#define TERMINAL_TX_BUF_SZ 20000
#define TERMINAL_RX_BUF_SZ 20000
#define TRAIN_TX_BUF_SZ 2000
#define TRAIN_RX_BUF_SZ 100

/**
 * Generic transmit server implementation.
 * Continuously Receive()s, processes Getc()s from clients and messages that the
 * UART has received a character from a notifier. Then sends to the UART, should there be a byte to send.
 *
 * @param buf_sz         The maximum buffer size for transmitting.
 *                       Asserts that the buffer does not overflow.
 * @param channel        TRAIN/TERMINAL.
 * @param nottifier_tid  Notifier to reply to.
 */
void generic_tx_server(uint16_t buf_sz, int channel, int notifier_tid);

/**
 * Generic receive server implementation.
 * Continuously Receive()s, processes Getc()s from clients and messages that the
 * UART has received a character from a notifier. Then reads from the UART, should there be a byte.
 *
 * @param buf_sz         The maximum buffer size for receiving.
 *                       Asserts that the buffer does not overflow.
 * @param channel        TRAIN/TERMINAL.
 */
void generic_rx_server(uint16_t buf_sz, int channel);

/**
 * Registers the train tx server under 'TrainTxServer',
 * creates the 'TrainTxNotifier' (with higher priority),
 * then calls the generic server implementation.
 */
void train_tx_server();

/**
 * Registers the train tx server under 'TrainRxServer',
 * creates the 'TrainRxNotifier' (with higher priority),
 * then calls the generic server implementation.
 */
void train_rx_server();

/**
 * Registers the terminal tx server under 'TerminalTxServer',
 * creates the 'TerminalTxNotifier' (with higher priority),
 * then calls the generic server implementation.
 */
void terminal_tx_server();

/**
 * Registers the terminal rx server under 'TerminalRxServer',
 * creates the 'TerminalRxNotifier' (with higher priority),
 * then calls the generic server implementation.
 */
void terminal_rx_server();

/**
 * Creates train_tx_server(), train_rx_server(), terminal_tx_server(), terminal_rx_server().
 */
void spawn_ioservers();

/**
 * Kills all train/terminal tx/rx servers/notifiers.
 * Gets their task id from the nameserver, i.e. expects the nameserver to be running.
 */
void kill_ioservers();

#endif /* IOSERVER_H */
