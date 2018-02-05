/**
 * @file
 * @brief Definitions describing the versatilepb registers
 */
#ifndef TRAINS_VERSATILEPB_H
#define TRAINS_VERSATILEPB_H

// clock base 0x101E2000
#define TIMER1_BASE         0x101E2000
#define TIMER2_BASE         0x101E3000
#define LDR_OFFSET          0x00000000
#define VAL_OFFSET          0x00000004
#define CTRL_OFFSET         0x00000008
#define CLR_OFFSET          0x0000000C
#define TIMER_SIZE          0x00000002 // 0: 16 bit (default), 1: 32 bit
#define ENABLE_INTERRUPT    0x00000020 // 1: isEnabled (default: 1)
#define MODE_MASK           0x00000040 // 0: free-running (default), 1: periodic
#define ENABLE_MASK         0x00000080 // written to in control register
#define CLR_OFFSET          0x0000000C

// http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183f/DDI0183.pdf
// Train
#define UART0_BASE          0x101f1000

// Terminal
#define UART1_BASE          0x101f2000

#define UART_DATA_OFFSET    0x0
#define UART_RSR_OFFSET     0x4
#define UART_FLAG_OFFSET    0x018
#define UARTIBRD_OFFSET     0x024 // Integer baud rate register
#define UART_LCRH_OFFSET    0x02C
#define UART_CTLR_OFFSET    0x30

#define RXFE_MASK           0x10
#define TXFF_MASK           0x20
#define RXFF_MASK           0x40
#define TXFE_MASK           0x80
#define TXBUSY_MASK         0x08 // TODO
#define CTS_MASK            0x00 // TODO

#define DATA_MASK           0xFF
#define UART_ERR_MASK       0xF00 // bit 11 OE, 10 BE, 9 PE, 8 FE

#define FE_MASK             0x1
#define PE_MASK             0x2
#define BE_MASK             0x4
#define OE_MASK             0x8

#define UART_CR             0x030 // control

#define WLEN_MASK           0x60
#define FEN_MASK            0x10
#define STP2_MASK           0x08
#define PEN_MASK            0x01

#define UARTIMSC_OFFSET     0x038
#define UARTRIS_OFFSET      0x03C
#define UARTMIS_OFFSET      0x040 // Read-only
#define UARTICR_OFFSET      0x044

// For the masked interrupt status register, UARTMIS
// For clearing interrupts in UARTICR
#define UARTCTSMIC_MASK     0x001 // Note we don't use this in versatilepb
#define UARTRXINTR_MASK     0x010
#define UARTTXINTR_MASK     0x020
#define UARTERRORS_MASK     0x780

#define VIC1_BASE           0x10140000
#define VIC2_BASE           0x1000C000
#define VIC1_ENABLE_OFFSET  0x10
#define VIC2_ENABLE_OFFSET  0x08
#define VIC1_INTCLR_OFFSET  0x14
#define VIC2_INTCLR_OFFSET  0xC
#define VIC1_TIMER_MASK     0x20
#define VIC1_UART0_MASK     0x1000
#define VIC1_UART1_MASK     0x2000

#endif /* TRAINS_VERSATILEPB_H */



///////////////////////////////////
// #define TXBUSY_MASK         0x8
// #define FIFO_ERROR_MASK     0x700
// #define TX_FIFO_WIDTH 8 // TODO
// #define RX_FIFO_WIDTH 11 // TODO
// start bit - data bits(8, LSB first) - parity bit - stop bits
// #define FIFO_DEPTH 16 // TODO
