/**
 * @file
 * @brief Notifier implementations.
 */
#ifndef NOTIFIER_H
#define NOTIFIER_H

#include "codes.h"
#include "event_data.h"
#include "messages.h"
#include "tstdlib.h"


/**
 * A generic notifier implementation that simply calls AwaitEvent(to_wait_on) and Send(server_tid)
 * forever.
 *
 * @param to_wait_on event to call AwaitEvent() on.
 * @param server_tid the server to contact after returning from AwaitEvent.
 */
void generic_notifier(enum event_id to_wait_on, int server_tid);

/**
 * Notifier for the clock server.
 * Registers the notifier at 'ClockNotifier',
 * expects there to already be a clock server registered with the name 'ClockServer'
 */
void clock_notifier();

/**
 * Notifier for the train transmit server.
 * Registers the notifier at 'TrainTxNotifier',
 * expects there to already be a train transmit server registered with the name 'TrainTxServer'
 */
void train_tx_notifier();

/**
 * Notifier for the train receive server.
 * Registers the notifier at 'TrainRxNotifier',
 * expects there to already be a train receive server registered with the name 'TrainRxServer'
 */
void train_rx_notifier();

/**
 * Notifier for the terminal transmit server.
 * Registers the notifier at 'TerminalTxNotifier',
 * expects there to already be a terminal transmit server registered with the name 'TerminalTxServer'
 */
void terminal_tx_notifier();

/**
 * Notifier for the terminal receive server.
 * Registers the notifier at 'TerminalRxNotifier',
 * expects there to already be a terminal receive server registered with the name 'TerminalRxServer'
 */
void terminal_rx_notifier();

#endif /* NOTIFIER_H */
