/*
 * bwio.c - busy-wait I/O routines for diagnosis
 *
 * Specific to the TS-7200 ARM evaluation board
 *
 */

#include "bwio.h"
#include "../../test-resources/assert.h"

enum OUT_PORT {
  TERMINAL,
  TRAINS
};

// TODO rx

int oe_in_sensor = 0;

void empty_buf(int channel, bool out) {
  switch(channel) {
  case COM1: {
    if (out) {
      char_buffer_empty(&train_output_buf);
    } else {
      char_buffer_empty(&train_input_buf);
    }
    break;
  }
  case COM2: {
    if (out) {
      char_buffer_empty(&terminal_output_buf);
    } else {
      char_buffer_empty(&terminal_input_buf);
    }
    break;
  }
  }
}

void setup_io() {
  bwsetspeed(COM1, 2400);   // IMPORTANT: ALWAYS CALL SETSPEED BEFORE SETFIFO
  bwsetspeed(COM2, 115200); // (SEE CIRRUS MANUAL p350/351 for more info)
  bwsetfifo(COM1, OFF);
  bwsetfifo(COM2, OFF);
  char_buffer_init(&train_input_buf, train_input_chars, IOBUFFERSZ);
  char_buffer_init(&train_output_buf, train_output_chars, IOBUFFERSZ);
  char_buffer_init(&terminal_input_buf, terminal_input_chars, IOBUFFERSZ);
  char_buffer_init(&terminal_output_buf, terminal_output_chars, IOBUFFERSZ);
}


int trysendall() {
  bwtrysendbyte(COM1);
  return !bwtrysendbyte(COM2);
}

int tryreceiveall() {
  bwtryreceivebyte(COM1);
  return !bwtryreceivebyte(COM2);
}

void dumpbuf(char_buffer *buf) {
  for(unsigned int i = 0; i < buf->elems; i++) {
    bwputc(COM2, buf->data[i]);
  }
}

int bwsendbyte_buffered(int channel, char c) {
  char_buffer *buf;
  switch(channel) {
    case COM1: {
      buf = &train_output_buf;
      break;
    }
    case COM2: {
      buf = &terminal_output_buf;
      break;
    }
    default:
      return -1;
  }
  if (char_buffer_is_full(buf)) {
    bwputc(COM2, 'X');
    bwputc(COM2, 'X');
    bwputc(COM2, 'X');
    bwputc(COM2, 'X');
    bwputc(COM2, 'X');
    dumpbuf(buf);
    return -2;
  }
  char_buffer_put(buf, c);
  return 0;
}

uint16_t bwgetnumreadable_bytes(int channel) {
  char_buffer *buf;
  switch(channel) {
    case COM1: {
      buf = &train_input_buf;
      break;
    }
    case COM2: {
      buf = &terminal_input_buf;
      break;
    }
  }
  return char_buffer_get_num_elems(buf);
}

bool bwcanreadbyte_buffered(int channel) {
  char_buffer *buf;
  switch(channel) {
    case COM1: {
      buf = &train_input_buf;
      // printf("ELEM COM1: %d", buf->elems);
      break;
    }
    case COM2: {
      buf = &terminal_input_buf;
      break;
    }
  }
  return !char_buffer_is_empty(buf);
}

char bwreadbyte_buffered(int channel) {
  char_buffer *buf;
  switch(channel) {
    case COM1: {
      buf = &train_input_buf;
      break;
    }
    case COM2: {
      buf = &terminal_input_buf;
      break;
    }
  }
  return char_buffer_get(buf);
}

int bwtrysendbyte(int channel) {
  char_buffer *buf;
  switch(channel) {
    case COM1: {
      buf = &train_output_buf;
      break;
    }
    case COM2: {
      buf = &terminal_output_buf;
      break;
    }
    default:
      return -2;
  }
  while (bwcanputc(channel) && !char_buffer_is_empty(buf)) {
    bwputc(channel, char_buffer_get(buf));
  }
  return 0;
}

int bwtryreceivebyte(int channel) {
  char_buffer *buf;
  switch(channel) {
    case COM1: {
      buf = &train_input_buf;
      break;
    }
    case COM2: {
      buf = &terminal_input_buf;
      break;
    }
    default: {
      return -2;
    }
  }
  while (bwcangetc(channel)) {
    if (char_buffer_is_full(buf)) {
      bwputc(COM2, 'Y');
      bwputc(COM2, 'Y');
      bwputc(COM2, 'Y');
      bwputc(COM2, 'Y');
      bwputc(COM2, 'Y');
      dumpbuf(buf);
      return -3;
    }
    char c = bwgetc(channel);
    char_buffer_put(buf, c);
      /*if (channel == COM1) {
      for(int i = 0; i < 8; i++) {
	bwputc(COM2, (c & (1 << (8-i-1))) ? '1' : '0');
      }
      }*/
  }
  return 0;
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
int bwsetfifo(int channel, int state) {
  int *line, buf;
  switch (channel) {
    case COM1:
      line = (int *) (UART1_BASE + UART_LCRH_OFFSET);
      break;
    case COM2:
      line = (int *) (UART2_BASE + UART_LCRH_OFFSET);
      break;
    default:
      return -1;
  }
  buf = *line;
  /**
   * TRAIN: 1 start bit, 2 stop bits, no parity, 8 data bits
   */
  if (channel == COM1) {
    buf |= STP2_MASK;
    buf = buf & ~PEN_MASK; // no parity bit!
    buf |= WLEN_MASK;
    }
  buf = state ? (buf | FEN_MASK) : (buf & ~FEN_MASK);
  *line = buf;
  return 0;
}

int bwsetspeed(int channel, int speed) {
  int *high, *low;
  switch (channel) {
    case COM1:
      high = (int *) (UART1_BASE + UART_LCRM_OFFSET);
      low = (int *) (UART1_BASE + UART_LCRL_OFFSET);
      break;
    case COM2:
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
      *low = 0xBF; //0xC0; // 115200 * 4 / 2400 = 192 -> 0xC0 TODO
      return 0;
    default:
      return -1;
  }
}

int bwcanputc(int channel) {
  assert(channel == COM1 || channel == COM2);
  volatile int *flags;
  switch (channel) {
    case COM1:
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      break;
    case COM2:
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
      break;
  }
  if (COM1 == channel) {
    return !(*flags & (TXFF_MASK | TXBUSY_MASK) && (*flags & CTS_MASK));
  }
  return !(*flags & (TXFF_MASK | TXBUSY_MASK));
}

int bwcangetc(int channel) {
  assert(channel == COM1 || channel == COM2);
  volatile int *flags;
  switch (channel) {
    case COM1:
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      break;
    case COM2:
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
      break;
  }
  return *flags & RXFF_MASK;
}

// TODO
int bwputc_fifo(int channel, char c) {
#define RXFE_MASK           0x10   // Receive buffer empty
#define TXFF_MASK           0x20   // Transmit buffer full
#define RXFF_MASK           0x40   // Receive buffer full
#define TXFE_MASK           0x80   // Transmit buffer empty
  (void)c;
  return channel;
}

int bwputc(int channel, char c) {
  volatile int *flags, *data; // just to be safe
  switch (channel) {
    case COM1:
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
      break;
    case COM2:
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART2_BASE + UART_DATA_OFFSET);
      break;
    default:
      return -1;
  }
  // TXFF: Transmit buffer full
  if (COM1 == channel) {
    while ((*flags & TXFF_MASK) || !(*flags & CTS_MASK)); // CHECK THAT THIS DOESN'T GET OPTIMIZED AWAY
  } else {
    while ((*flags & TXFF_MASK));
  }
  *data = c;
  return 0;
}

char c2x(char ch) {
  if ((ch <= 9)) {
    return '0' + ch;
  } else {
    return 'a' + ch - 10;
  }
}

int bwputx(int channel, char c) {
  char chh, chl;

  chh = c2x(c / 16);
  chl = c2x(c % 16);
  bwsendbyte_buffered(channel, chh);
  return bwsendbyte_buffered(channel, chl);
  // bwputc(channel, chh);
  // return bwputc(channel, chl);
}

int bwputr(int channel, unsigned int reg) {
  int byte;
  char *ch = (char *) &reg;

  for (byte = 3; byte >= 0; byte--) {
    bwputx(channel, ch[byte]);
  }
  return bwsendbyte_buffered(channel, ' ');
  //return bwputc(channel, ' ');
}

int bwputstr(int channel, char *str) {
  while (*str) {
    if (bwsendbyte_buffered(channel, *str) < 0) {
    // if (bwputc(channel, *str) < 0) {
      return -1;
    } else {
      str++;
    }
  }
  return 0;
}

void bwputw(int channel, int n, char fc, char *bf) {
  char ch;
  char *p = bf;

  while (*p++ && n > 0) n--;
  while (n-- > 0) bwsendbyte_buffered(channel, fc);
  while ((ch = *bf++)) bwsendbyte_buffered(channel, ch);
  // while (n-- > 0) bwputc(channel, fc);
  // while ((ch = *bf++)) bwputc(channel, ch);
}

uint16_t last_err = 0;
bool got_err = false;

int bwgetc(int channel) {
  volatile int *data;
  volatile unsigned char c;

  switch (channel) {
    case COM1:
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
      break;
    case COM2:
      data = (int *) (UART2_BASE + UART_DATA_OFFSET);
      break; // TODO UART_RSR_OFFSET for status
    default:
      return -1;
  }
  // while (!(*flags & RXFF_MASK)); // TODO use buffer
  if (channel == COM1) {
    volatile int *rcv1 = (int *)(UART1_BASE + UART_RSR_OFFSET);
    if ((*rcv1) & (OE_MASK | BE_MASK | PE_MASK | FE_MASK)) {
#if DEBUG
      printf("\033[?25h\033[2;1H\033[K");
      printf("OE: %d, BE: %d, PE: %d, FE: %d", (*rcv1) & OE_MASK, (*rcv1) & BE_MASK, (*rcv1) & PE_MASK, (*rcv1) & FE_MASK);
#endif      
      oe_in_sensor = 1;
      last_err = get_time();
      got_err = true;
      volatile int *stat = (int *)(UART1_BASE + UART_RSR_OFFSET);
      *stat = 0;
      //if (char_buffer_get_num_elems(&train_input_buf) % 2 == 0) {
      //return; // TODO
      //}
    }
    if (got_err && last_err + (uint16_t)500 < (uint16_t)get_time()) {
#if DEBUG      
      printf("\033[?25h\033[2;1H\033[K");
#endif      
      got_err = false;
    }
  }
  c = *data; // TODO check error etc
  return c;
}

int bwa2d(char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  return -1;
}

char bwa2i(char ch, char **src, int base, int *nump) {
  int num, digit;
  char *p;

  p = *src;
  num = 0;
  while ((digit = bwa2d(ch)) >= 0) {
    if (digit > base) break;
    num = num * base + digit;
    ch = *p++;
  }
  *src = p;
  *nump = num;
  return ch;
}

void bwui2a(unsigned int num, unsigned int base, char *bf) {
  int n = 0;
  int dgt;
  unsigned int d = 1;

  while ((num / d) >= base) d *= base;
  while (d != 0) {
    dgt = num / d;
    num %= d;
    d /= base;
    if (n || dgt > 0 || d == 0) {
      *bf++ = dgt + (dgt < 10 ? '0' : 'a' - 10);
      ++n;
    }
  }
  *bf = 0;
}

void bwi2a(int num, char *bf) {
  if (num < 0) {
    num = -num;
    *bf++ = '-';
  }
  bwui2a(num, 10, bf);
}

void bwformat(int channel, char *fmt, va_list va) {
  char bf[12];
  char ch, lz;
  int w;

  while ((ch = *(fmt++))) {
    if (ch != '%')
      bwsendbyte_buffered(channel, ch);
      // bwputc(channel, ch);
    else {
      lz = 0;
      w = 0;
      ch = *(fmt++);
      switch (ch) {
        case '0':
          lz = 1;
          ch = *(fmt++);
          break;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
          ch = bwa2i(ch, &fmt, 10, &w);
          break;
      }
      switch (ch) {
        case 0:return;
        case 'c':
          bwsendbyte_buffered(channel, va_arg(va, char));
          // bwputc(channel, va_arg(va, char));
          break;
        case 's':
          bwputw(channel, w, 0, va_arg(va, char *));
          break;
        case 'u':
          bwui2a(va_arg(va, unsigned int), 10, bf);
          bwputw(channel, w, lz, bf);
          break;
        case 'd':
          bwi2a(va_arg(va, int), bf);
          bwputw(channel, w, lz, bf);
          break;
        case 'x':
          bwui2a(va_arg(va, unsigned int), 16, bf);
          bwputw(channel, w, lz, bf);
          break;
        case '%':
          bwsendbyte_buffered(channel, ch);
          // bwputc(channel, ch);
          break;
      }
    }
  }
}

void bwprintf(int channel, char *fmt, ...) {
  va_list va;

  va_start(va, fmt);
  bwformat(channel, fmt, va);
  va_end(va);
}

int rxd_data(int channel) {
  return channel;
}
