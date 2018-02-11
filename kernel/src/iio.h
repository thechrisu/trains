/**
 * @file
 * @brief Interrupt-driven I/O functions.
 */
#ifndef IIO_H
#define IIO_H

#include "kassert.h"
#include "io.h"
#include "vic.h"

void enable_uart_event(enum event_id e);
int receive_interrupt_clear(int channel);
void enable_transmit_interrupt(int channel, int enable);
void enable_uart_interrupt(int channel, register_t enable_mask, int enable);
int try_clear_train_send();
void interrupt_modem_clear();
void interrupt_rx_clear(int channel);
void interrupt_tx_clear(int channel);
int get_modem_interrupt_bits();

#endif /* IIO_H */
