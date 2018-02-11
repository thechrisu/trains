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

// TODO document these!
void clock_notifier();
void train_tx_notifier();
void train_rx_notifier();
void terminal_tx_notifier();
void terminal_rx_notifier();

#endif /* NOTIFIER_H */
