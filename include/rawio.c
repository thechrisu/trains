#include "rawio.h"

char raw_get_error(int channel) {
  volatile int *ERROR_REG;
#if VERSATILEPB
  switch (channel) {
  case TRAIN:
    ERROR_REG = (int *)UART0_BASE;
    break;
  case TERMINAL:
    ERROR_REG = (int *)UART1_BASE;
    break;
  default:
    return 0;
  }
  return (*ERROR_REG & UART_ERR_MASK) >> 8;
#else
  switch (channel) {
  case TRAIN:
    ERROR_REG = (int *)UART1_BASE;
    break;
  case TERMINAL:
    ERROR_REG = (int *)UART2_BASE;
    break;
  default:
    return 0;
  }
  int err = *(int *)(ERROR_REG + UART_RSR_OFFSET);
  if (err) {
    *(ERROR_REG + UART_RSR_OFFSET) = 0; // RESET
  }
  return err & 0xF;
#endif
}

int rawputc(int channel, char c) {
  volatile int *data; // just to be safe
  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      data = (int *) (UART0_BASE + UART_DATA_OFFSET);
#else
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
#else
      data = (int *) (UART2_BASE + UART_DATA_OFFSET);
#endif
      break;
    default:
      return -1;
  }
  // TXFF: Transmit buffer full
  *data = c;
  return 0;
}

int rawgetc(int channel, char *c) {
  volatile int *data;

  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      data = (int *) UART0_BASE + UART_DATA_OFFSET;
#else
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      data = (int *) UART1_BASE + UART_DATA_OFFSET;
#else
      data = (int *) (UART2_BASE + UART_DATA_OFFSET);
#endif
      break;
    default:
      return -1;
  }
  volatile int d = *data;
  *c = d & DATA_MASK;
  if (channel == TRAIN) {
    /*#if FIFOS
    return (d & ((OE_MASK | BE_MASK | PE_MASK | FE_MASK) << 8)) >> 8;
    #else*/
    return raw_get_error(channel) & (OE_MASK | BE_MASK | PE_MASK | FE_MASK);
    // #endif /* FIFOS */
  }
  return 0;
}

int rawcanputc(int channel) {
  volatile int *flags;
  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      flags = (int *) (UART0_BASE + UART_FLAG_OFFSET);
#else
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
#else
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
#endif
      break;
    default:
      return 0;
  }
#ifndef VERSATILEPB
  if (TRAIN == channel) {
    volatile int txff = *flags & TXFF_MASK;
    volatile int cts = *flags & CTS_MASK;
    volatile int r = !txff && cts;
    if (!r) {
      logprintf("Canputc for %d, TXFF: %x, CTS: %x\n\r", txff, cts);
    }
    return r;
  }
#endif /* VERSATILEPB */
  return !(*flags & TXFF_MASK);
}

int rawcangetc(int channel) {
  volatile int *flags;
  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      flags = (int *) (UART0_BASE + UART_FLAG_OFFSET);
#else
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
#else
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
#endif
      break;
    default:
      return 0;
  }
  return !(*flags & RXFE_MASK);
}
