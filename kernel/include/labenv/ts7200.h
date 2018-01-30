/**
 * @file
 * @brief ts7200.h - definitions describing the ts7200 peripheral registers
 *
 * Specific to the TS-7200 ARM evaluation board
 *
 */


// 32 bit timer
#define TIMER3_CLK           0x8         // 1: 508kHz, 0: 2kHz. default: 0
#define TIMER3_ENABLE        0x80
#define TIMER3_MODE          0x40        // 1: periodic (reloads val), 0: free running (0xFFFFFFFF)
#define TIMER3_LOAD          0x80810080  // 32bits
#define TIMER3_VALUE         0x80810084  // 32 bits
#define TIMER3_CTRL          0x80810088  // 32 bits
#define TIMER3_CLEAR         0x8081008C  // 1 bit

// 40 bit timer
#define TIMER4_VAL_LO        0x80810060  // 32 bits
#define TIMER4_HI_ONOFF      0x80810064  // 9 bits (technically unsigned short)
#define TIMER4_HI_VAL_MSK    0xFF
#define TIMER4_ONOFF_MSK     0x100       // 1: Enable, 0: disable timer


#define TIMER1_BASE         0x80810000
#define TIMER2_BASE         0x80810020
#define TIMER3_BASE         0x80810080

#define LDR_OFFSET          0x00000000    // 16/32 bits, RW
#define VAL_OFFSET          0x00000004    // 16/32 bits, RO
#define CTRL_OFFSET         0x00000008    // 3 bits, RW
#define ENABLE_MASK         0x00000080
#define MODE_MASK           0x00000040
#define CLKSEL_MASK         0x00000008
#define CLR_OFFSET          0x0000000c    // no data, WO

#define LED_ADDRESS         0x80840020
#define LED_NONE            0x0
#define LED_GREEN           0x1
#define LED_RED             0x2
#define LED_BOTH            0x3

// #define COM1                0 // train    port 0x03F8
// #define COM2                1 // terminal      0x02F8

#define IRDA_BASE           0x808b0000
#define UART1_BASE          0x808c0000
#define UART2_BASE          0x808d0000

// All the below registers for UART1
// First nine registers (up to Ox28) for UART 2

#define UART_DATA_OFFSET    0x0    // low 8 bits
#define DATA_MASK           0xff
#define UART_RSR_OFFSET     0x4    // low 4 bits
#define FE_MASK             0x1
#define PE_MASK             0x2
#define BE_MASK             0x4
#define OE_MASK             0x8
#define UART_LCRH_OFFSET    0x8    // low 7 bits
#define BRK_MASK            0x1
#define PEN_MASK            0x2    // parity enable
#define EPS_MASK            0x4    // even parity
#define STP2_MASK           0x8    // 2 stop bits
#define FEN_MASK            0x10   // fifo
#define WLEN_MASK           0x60   // word length
#define UART_LCRM_OFFSET    0xc    // low 8 bits
#define BRDH_MASK           0xff   // MSB of baud rate divisor

// baud rate divisor, transmission params, word length, buffer mode, # stop bits, parity mode, break generation
#define UART_LCRL_OFFSET    0x10   // low 8 bits
#define BRDL_MASK           0xff   // LSB of baud rate divisor
#define UART_CTLR_OFFSET    0x14   // low 8 bits
#define UARTEN_MASK         0x1
#define MSIEN_MASK          0x8    // modem status int
#define RIEN_MASK           0x10   // receive int
#define TIEN_MASK           0x20   // transmit int
#define RTIEN_MASK          0x40   // receive timeout int
#define LBEN_MASK           0x80   // loopback
#define UART_FLAG_OFFSET    0x18   // low 8 bits
#define CTS_MASK            0x1
#define DCD_MASK            0x2
#define DSR_MASK            0x4
#define TXBUSY_MASK         0x8
#define RXFE_MASK           0x10   // Receive buffer empty
#define TXFF_MASK           0x20   // Transmit buffer full
#define RXFF_MASK           0x40   // Receive buffer full
#define TXFE_MASK           0x80   // Transmit buffer empty
#define UART_INTR_OFFSET    0x1c
#define UART_DMAR_OFFSET    0x28

// Specific to UART1

#define UART_MDMCTL_OFFSET  0x100
#define UART_MDMSTS_OFFSET  0x104
#define UART_HDLCCTL_OFFSET 0x20c
#define UART_HDLCAMV_OFFSET 0x210
#define UART_HDLCAM_OFFSET  0x214
#define UART_HDLCRIB_OFFSET 0x218
#define UART_HDLCSTS_OFFSET 0x21c

#define FIFO_ERROR_MASK     0x700

#define TX_FIFO_WIDTH 8
#define RX_FIFO_WIDTH 11

// start bit - data bits(8, LSB first) - parity bit - stop bits
#define FIFO_DEPTH 16

// VIC constants
#define VIC_BASE            0x800B0000
#define VIC_ENABLE_OFFSET   0x10
#define VIC_TIMER_MASK      0x10
