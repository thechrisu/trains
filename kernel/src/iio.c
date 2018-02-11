#include "iio.h"

int uart0_cts_asserted;
int uart0_txfe_asserted;

void maybe_received_cts() {
#ifndef TESTING
  volatile register_t *train_uart = (register_t*)(UART1_BASE + UART_FLAG_OFFSET);
  uart0_cts_asserted |= (*train_uart) & CTS_MASK;
#endif /* TESTING */
}

int try_clear_train_send() {
#if VERSATILEPB
  return 1; // We don't have CTS in QEMU
#else
  if (uart0_txfe_asserted && uart0_cts_asserted) {
    uart0_txfe_asserted = 0;
    uart0_cts_asserted = 0;
    return 1;
  } else {
    return 0;
  }
#endif /* VERSATILEPB */
}

void enable_uart_interrupt(int channel, register_t enable_mask, int enable) {
#ifndef TESTING
  volatile register_t *reg;
#if VERSATILEPB
  switch (channel) {
  case TERMINAL:
    reg = (register_t *)UART1_BASE;
    break;
  case TRAIN:
    reg = (register_t *)UART0_BASE;
    break;
  }
  reg += UARTIMSC_OFFSET >> 2;
#else
  switch (channel) {
  case TERMINAL:
    reg = (register_t *)UART2_BASE;
    break;
  case TRAIN:
    reg = (register_t *)UART1_BASE;
    break;
  }
  reg += UART_CTLR_OFFSET >> 2;
#endif /* VERSATILEPB */
  if (enable) {
    *reg |= enable_mask;
  } else {
    *reg &= ~enable_mask;
  }
#endif /* TESTING */
}

void enable_uart_event(enum event_id e) {
  int enable = 1;
#ifndef TESTING
#ifdef VERSATILEPB
  switch (e) {
    case TRAIN_TX_INTERRUPT:
      enable_uart_interrupt(TRAIN, UARTTXINTR_MASK, enable);
      break;
    case TRAIN_RX_INTERRUPT:
      enable_uart_interrupt(TRAIN, UARTRXINTR_MASK, enable);
      break;
    case TERMINAL_TX_INTERRUPT:
      enable_uart_interrupt(TERMINAL, UARTTXINTR_MASK, enable);
      break;
    case TERMINAL_RX_INTERRUPT:
      enable_uart_interrupt(TERMINAL, UARTRXINTR_MASK, enable);
      break;
    default:
      break;
  }
#else
  switch (e) {
    case TRAIN_TX_INTERRUPT:
      enable_uart_interrupt(TRAIN, UARTTXENABLE_MASK, enable);
      break;
    case TRAIN_RX_INTERRUPT:
      enable_uart_interrupt(TRAIN, UARTRXENABLE_MASK, enable);
      break;
    case TERMINAL_TX_INTERRUPT:
      enable_uart_interrupt(TERMINAL, UARTTXENABLE_MASK, enable);
      break;
    case TERMINAL_RX_INTERRUPT:
      enable_uart_interrupt(TERMINAL, UARTRXENABLE_MASK, enable);
      break;
    default:
      break;
  }
#endif /* VERSATILEPB */
#endif /* TESTING */
}

void interrupt_tx_clear(int channel) {
#ifndef TESTING
#if VERSATILEPB
  enable_uart_interrupt(channel, UARTTXINTR_MASK, false);
#else
  enable_uart_interrupt(channel, UARTTXENABLE_MASK, false);
  if (channel == TRAIN) {
    uart0_txfe_asserted = 1;
  }
#endif /* VERSATILEPB */
#endif /* TESTING */
}

void interrupt_rx_clear(int channel) {
#ifndef TESTING
#if VERSATILEPB
  enable_uart_interrupt(channel, UARTRXINTR_MASK, false);
#else
  enable_uart_interrupt(channel, UARTRXENABLE_MASK, false);
#endif /* VERSATILEPB */
#endif /* TESTING */
}

void interrupt_modem_clear() {
#if !VERSATILEPB && !TESTING
  volatile register_t *reg = (register_t *)(UART1_BASE + UART_INTR_OFFSET);
  *reg = 1;
#endif /* VERSATILEPB */
}

int get_modem_interrupt_bits() {
#if TESTING
  return 1;
#else
#ifdef VERSATILEPB
  return 0;
#else
  return *(register_t*)VIC2_BASE & VIC2_UART1MOD_MASK;
#endif /* VERSATILEPB */
#endif /* TESTING */
}
