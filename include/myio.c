/*
 * bwio.c - busy-wait I/O routines for diagnosis
 *
 * Specific to the TS-7200 ARM evaluation board
 *
 */

#include "myio.h"

extern int uart0_txfe_asserted;
extern int uart0_cts_asserted;

// Dear coding gods, please forgive me
void nop() {
  __asm__ volatile
    (
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     "nop\n\t"
     );
}

char log_buffer[LOG_BUFFER_SIZE];
uint32_t log_index;

void enable_io_interrupts(int enable) {
#ifdef VERSATILEPB
  enable_uart_interrupt(TRAIN, UARTTXINTR_MASK, enable);
  enable_uart_interrupt(TERMINAL, UARTTXINTR_MASK, enable);
  nop();
  enable_uart_interrupt(TRAIN, UARTRXINTR_MASK, enable);
  enable_uart_interrupt(TERMINAL, UARTRXINTR_MASK, enable);
#if FIFOS && IOINTERRUPTS
  nop();
  enable_uart_interrupt(TERMINAL, UARTRTINTR_MASK, enable);
#endif /* FIFOS && IOINTERRUPTS */
#else
  enable_uart_interrupt(TRAIN, UARTTXENABLE_MASK, enable);
  enable_uart_interrupt(TERMINAL, UARTTXENABLE_MASK, enable);
  nop();
  enable_uart_interrupt(TRAIN, UARTRXENABLE_MASK, enable);
  enable_uart_interrupt(TERMINAL, UARTRXENABLE_MASK, enable);
  nop();
  enable_uart_interrupt(TRAIN, UARTMIENABLE_MASK, enable);
#if FIFOS && IOINTERRUPTS
  nop();
  enable_uart_interrupt(TRAIN, UARTRTENABLE_MASK, enable);
#endif /* FIFOS && IOINTERRUPTS */
#endif /* VERSATILEPB */
  nop();
}

void setup_io() {
  setspeed(TRAIN, 2400);   // IMPORTANT: ALWAYS CALL SETSPEED BEFORE SETFIFO
  nop();
  setspeed(TERMINAL, 115200); // (SEE CIRRUS MANUAL p350/351 for more info)
  nop();
  setfifo(TRAIN, OFF);
  nop();
#if FIFOS && IOINTERRUPTS
  setfifo(TERMINAL, ON);
#else
  setfifo(TERMINAL, OFF);
#endif /* FIFOS */
  nop();
  raw_get_error(TRAIN); // Just to make sure we don't cause it...
}

void setup_iio() {
  enable_io_interrupts(true);
  uart0_txfe_asserted = 0;
  uart0_cts_asserted = 0;
}

void desetup_iio() {
  enable_io_interrupts(false);
}

////////////////////////////////////////////////////////////////////////

/*
 *
 * The UARTs are initialized by RedBoot to the following state
 * 	115,200 bps
 * 	8 bits
 * 	no parity
 * 	fifos enabled
 */
int setfifo(int channel, int state) {
  int *line, buf;
  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      line = (int *) (UART0_BASE + UART_LCRH_OFFSET);
#else
      line = (int *) (UART1_BASE + UART_LCRH_OFFSET);
#endif /* VERSATILEPB */
      break;
    case TERMINAL:
#if VERSATILEPB
      line = (int *) (UART1_BASE + UART_LCRH_OFFSET);
#else
      line = (int *) (UART2_BASE + UART_LCRH_OFFSET);
#endif /* VERSATILEPB */
      break;
    default:
      return -1;
  }
  buf = *line;
  /**
   * TRAIN: 1 start bit, 2 stop bits, no parity, 8 data bits
   */
  if (channel == TRAIN) {
    buf |= STP2_MASK;
    buf = buf & ~PEN_MASK; // no parity bit!
    buf |= WLEN_MASK;
  }
  buf = state ? (buf | FEN_MASK) : (buf & ~FEN_MASK);
#if VERSATILEPB
  if (state && channel == TERMINAL) {
    *((int *)UART1_BASE + UARTIFLS_OFFSET) |= (2 << 3) | 2; // half full
  }
#endif /* VERSATILEPB */
  *line = buf;
  return 0;
}

int setspeed(int channel, int speed) {
#if VERSATILEPB
  // BAUDDIV = (FUARTCLK/ {16 * Baud rate})
  // UART clock frequency of 7.3728MHz
  // 115200 --> 0x4
  // 2400 --> 0xC0
  volatile unsigned int *UARTIBRD_REG;
  switch (channel) {
    case TRAIN:
      UARTIBRD_REG = (unsigned int *)UART0_BASE + UARTIBRD_OFFSET;
      break;
    case TERMINAL:
      UARTIBRD_REG = (unsigned int *)UART1_BASE + UARTIBRD_OFFSET;
      break;
    default:
      return -1;
  }
  switch (speed) {
    case 115200:
      *UARTIBRD_REG = 0x04;
      return 0;
    case 2400:
      *UARTIBRD_REG = 0xC0;
      return 0;
    default:
      return -1;
  }
#else
  volatile int *high, *low;
  switch (channel) {
    case TRAIN:
      high = (int *) (UART1_BASE + UART_LCRM_OFFSET);
      low = (int *) (UART1_BASE + UART_LCRL_OFFSET);
      break;
    case TERMINAL:
      high = (int *) (UART2_BASE + UART_LCRM_OFFSET);
      low = (int *) (UART2_BASE + UART_LCRL_OFFSET);
      break;
    default:
      return -1;
  }
  /**
   * Clocks: 1.8432 MHZ / 7.3728 MHZ
   */
  switch (speed) {
    case 115200: // used for terminal
      *high = 0x0;
      *low = 0x3;
      return 0;
    case 2400: // used for train set
      *high = 0x0;
      *low = 0xBF; // 115200 * 4 / 2400 = 192 -> 0xC0
      return 0;
    default:
      return -1;
  }
#endif
}

int terminalputc(char c) {
  return sendbyte_buffered(TERMINAL, c);
}

int bwterminalputc(char c) {
  return putc(TERMINAL, c);
}

int trainputc(char c) {
  return sendbyte_buffered(TRAIN, c);
}

int bwtrainputc(char c) {
  return putc(TRAIN, c);
}

int logputc(char c) {
  if (log_index < LOG_BUFFER_SIZE) {
    log_buffer[log_index++] = c;
  }
  return 0;
}

void printf(char *fmt, ...) {
  va_list va;

  va_start(va, fmt);
  format(bwterminalputc, fmt, va);
  va_end(va);
}

void bwprintf(char *fmt, ...) {
  va_list va;

  va_start(va, fmt);
  format(bwterminalputc, fmt, va);
  va_end(va);
}

void logprintf(char *fmt, ...) {
  va_list va;

  va_start(va, fmt);
#if BWLOG
  format(bwterminalputc, fmt, va);
#else
  format(logputc, fmt, va);
#endif
  va_end(va);
}

int logputr(unsigned int reg) {
  return putr(logputc, reg);
}

void dump_logs() {
  for (uint32_t i = 0; i < log_index; i += 1) {
    bwterminalputc(log_buffer[i]);
  }
}
