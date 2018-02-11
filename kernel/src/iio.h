/**
 * @file
 * @brief Interrupt-driven I/O functions.
 */
#ifndef IIO_H
#define IIO_H

#include "kassert.h"
#include "io.h"
#include "vic.h"

/**
 * Looks up the interrupt(s) associated with this event id and enables the interrupt in the UART.
 *
 * @param e Event id to look up.
 */
void enable_uart_event(enum event_id e);

/**
 * The underlying function for <code>enable_uart_event</code>
 * that actually only ORs or AND NOT-s the register in the
 * UART corresponding to the given channel.
 * For the TS7200, this just enables/disables the interrupts.
 * For Versatile/PB, this really clears the interrupt.
 *
 * @param channel       Currently only supports TRAIN/TERMINAL.
 * @param enable_mask   The bit mask to apply to the register.
 * @param enable        0: Disable. 1: Enable.
 */
void enable_uart_interrupt(int channel, register_t enable_mask, int enable);

/**
 * Implementation of the state machine for the train transmit channel:
 * Checks if TXFE and CTS have BOTH been received so far.
 * If so, it resets the state machine
 * (see
 * <a href="http://www.cgl.uwaterloo.ca/wmcowan/teaching/cs452/w18/pdf/flowControl.pdf">
 *   this PDF
 * </a> for an explanation why there is a state machine in the first place).
 *
 * @return 1 if both have been received so far, 0 otherwise.
 */
int try_clear_train_send();

/**
 * This function should be called on receiving a modem interrupt from the train UART.
 * If the CTS bit is asserted in the UART, this function will update the state machine.
 */
void maybe_received_cts();

/**
 * If we have received an interrupt from the UART modem, we have to clear it.
 */
void interrupt_modem_clear();

/**
 * Clears the receive interrupt from a given channel.
 * For the TS7200, this really just disables the interrupts.
 * For Versatile/PB, this really clears the interrupt.
 *
 * @param channel TRAIN/TERMINAL.
 */
void interrupt_rx_clear(int channel);

/**
 * Clears the transmit interrupt for a given channel.
 * Semantics are the same as for <code>interrupt_rx_clear</code>,
 * but if we come from TRAIN in the TS7200, we update the state machine
 * to indicate that we have received the transmit interrupt.
 *
 * @param channel TRAIN/TERMINAL.
 */
void interrupt_tx_clear(int channel);

/**
 * Just reads the UART modem register of VIC2 in the TS7200.
 * @return 1 if TESTING enabled, 0 if compiling for Versatile/PB and the modem register bits otherwise.
 */
int get_modem_interrupt_bits();

#endif /* IIO_H */
