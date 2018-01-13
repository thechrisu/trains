//
// Created by Christoph Ulshöfer on 2018-01-12.
//

#include "bwio.h"
// see https://balau82.wordpress.com/2010/02/28/hello-world-for-bare-metal-arm-using-qemu/


// UART0 base 0x101F1000
// UART1 base 0x101F2000



void setup_io() {

}

void bwprintf() {

}

void bwputc() {

}

void bwsendbyte_buffered() {

}
// first write UARTIBRD, UARTFBRD, then UARTLCR_H

/*
 * DMA
15 UART0 Tx 14 UART0 Rx 13 UART1 Tx 12 UART1 Rx 11 UART2 Tx 10 UART2 Rx
DMA channels for UART3 Tx and Rx are selectable as 0,1, or 2. See Direct Memory Access Controller and mapping registers.
 */

// UART0 has 5-wire RS232 --> we use it for trains
// UART1 has 3-wire RS232 --> we use it for the terminal
volatile unsigned int * const UART0DR = (unsigned int *)0x101f1000;
volatile unsigned int * const UART1DR = (unsigned int *)0x101f2000;

void setup_uarts(int enable_fifo) {
  // UARTs are enabled by default (UART_CR bit 0)?
  volatile unsigned int *UARTBRD0 = UART0DR + UARTIBRD;
  *UARTBRD0 = 0xC0;
  volatile unsigned int *UARTBRD1 = UART1DR + UARTIBRD;
  *UARTBRD1 = 0x4;
  char basic_config = (WLEN | STP2_MASK) & ~PEN;
  volatile unsigned int *UART0LCRH = UART0DR + UARTLCR_H;
  volatile unsigned int *UART1LCRH = UART1DR + UARTLCR_H;
  *UART0LCRH = enable_fifo ? basic_config | FEN_MASK : basic_config & ~FEN_MASK;
  *UART1LCRH = enable_fifo ? basic_config | FEN_MASK : basic_config & ~FEN_MASK;
}


char get_uart0_error() {
  return (*UART0DR & UART_ERR_MASK) >> 8;
}

char get_uart1_error() {
  return (*UART1DR & UART_ERR_MASK) >> 8;
}

int uart0_hasbyte() {
  return (*UART0DR + UART_FR) & RXFF;
}

int uart1_hasbyte() {
  return (*UART1DR + UART_FR) & RXFF;
}

char uart0_getbyte() {
  return *UART0DR & 0xff;
}

char uart1_getbyte() {
  return *UART1DR & 0xff;
}

void uart0_putbyte(char c) {
  *UART0DR = (unsigned int)c;
}

void uart1_putbyte(char c) {
  *UART1DR = (unsigned int)c;
}

unsigned long long get_time() {

}

void print_uart0(char *s) {
  while (*s != '\0') {
    *UART0DR = *s++;
    *UART1DR = *s++;
  }
}

void c_entry() {
  print_uart0("Hello, world!\n");
}
