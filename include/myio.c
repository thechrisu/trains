/*
 * bwio.c - busy-wait I/O routines for diagnosis
 *
 * Specific to the TS-7200 ARM evaluation board
 *
 */

#include "myio.h"

char log_buffer[LOG_BUFFER_SIZE];
uint32_t log_index;

char_buffer train_input_buf, train_output_buf,
    terminal_input_buf, terminal_output_buf;

char train_input_chars[IOBUFFERSZ], train_output_chars[IOBUFFERSZ],
    terminal_input_chars[IOBUFFERSZ], terminal_output_chars[IOBUFFERSZ];

int oe_in_sensor = 0;

void empty_buf(int channel, bool out) {
  switch(channel) {
    case TRAIN: {
      if (out) {
        char_buffer_empty(&train_output_buf);
      } else {
        char_buffer_empty(&train_input_buf);
      }
      break;
    }
    case TERMINAL: {
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
  setspeed(TRAIN, 2400);   // IMPORTANT: ALWAYS CALL SETSPEED BEFORE SETFIFO
  setspeed(TERMINAL, 115200); // (SEE CIRRUS MANUAL p350/351 for more info)
  setfifo(TRAIN, OFF);
  setfifo(TERMINAL, OFF);
  char_buffer_init(&train_input_buf, train_input_chars, IOBUFFERSZ);
  char_buffer_init(&train_output_buf, train_output_chars, IOBUFFERSZ);
  char_buffer_init(&terminal_input_buf, terminal_input_chars, IOBUFFERSZ);
  char_buffer_init(&terminal_output_buf, terminal_output_chars, IOBUFFERSZ);
}


int trysendall() {
  trysendbyte(TRAIN);
  return !trysendbyte(TERMINAL);
}

int tryreceiveall() {
  tryreceivebyte(TRAIN);
  return !tryreceivebyte(TERMINAL);
}

void dumpbuf(char_buffer *buf) {
  for(unsigned int i = 0; i < buf->elems; i++) {
    putc(TERMINAL, buf->data[i]);
  }
}

int sendbyte_buffered(int channel, char c) {
  char_buffer *buf;
  switch(channel) {
    case TRAIN: {
      buf = &train_output_buf;
      break;
    }
    case TERMINAL: {
      buf = &terminal_output_buf;
      break;
    }
    default:
      return -1;
  }
  if (char_buffer_is_full(buf)) {
    putc(TERMINAL, 'X');
    putc(TERMINAL, '0' + channel);
    putc(TERMINAL, 'X');
    putc(TERMINAL, '0' + channel);
    putc(TERMINAL, 'X');
    putc(TERMINAL, '0' + channel);
    putc(TERMINAL, 'X');
    dumpbuf(buf);
    return -2;
  }
  char_buffer_put(buf, c);
  return 0;
}

uint16_t getnumreadable_bytes(int channel) {
  char_buffer *buf;
  switch(channel) {
    case TRAIN: {
      buf = &train_input_buf;
      break;
    }
    case TERMINAL: {
      buf = &terminal_input_buf;
      break;
    }
    default:
      assert(0);
      return 0xFFFF;
  }
  return char_buffer_get_num_elems(buf);
}

bool canreadbyte_buffered(int channel) {
  assert(channel == TRAIN || channel == TERMINAL);
  char_buffer *buf;
  switch(channel) {
    case TRAIN: {
      buf = &train_input_buf;
      break;
    }
    case TERMINAL: {
      buf = &terminal_input_buf;
      break;
    }
    default:
      assert(0);
      return 0xFFFFFFFF;
  }
  return !char_buffer_is_empty(buf);
}

char readbyte_buffered(int channel) {
  assert(channel == TRAIN || channel == TERMINAL);
  char_buffer *buf;
  switch(channel) {
    case TRAIN: {
      buf = &train_input_buf;
      break;
    }
    case TERMINAL: {
      buf = &terminal_input_buf;
      break;
    }
    default:
      return -1;
  }
  return char_buffer_get(buf);
}

int trysendbyte(int channel) {
  char_buffer *buf;
  switch(channel) {
    case TRAIN: {
      buf = &train_output_buf;
      break;
    }
    case TERMINAL: {
      buf = &terminal_output_buf;
      break;
    }
    default:
      return -2;
  }
  while (canputc(channel) && !char_buffer_is_empty(buf)) {
    char c = char_buffer_get(buf);
    putc(channel, c);
  }
  return 0;
}

int tryreceivebyte(int channel) {
  char_buffer *buf;
  switch(channel) {
    case TRAIN: {
      buf = &train_input_buf;
      break;
    }
    case TERMINAL: {
      buf = &terminal_input_buf;
      break;
    }
    default: {
      return -2;
    }
  }
  while (cangetc(channel)) {
    // putc(TERMINAL, cangetc(TERMINAL) ? 'T' : 'F');
    // putc(TRAIN, cangetc(TERMINAL) ? 'T' : 'F');
    if (char_buffer_is_full(buf)) {
      putc(TERMINAL, 'Y');
      putc(TERMINAL, '0' + channel);
      putc(TERMINAL, 'Y');
      putc(TERMINAL, '0' + channel);
      putc(TERMINAL, 'Y');
      putc(TERMINAL, '0' + channel);
      putc(TERMINAL, 'Y');
      dumpbuf(buf);
      return -3;
    }
    char c = getc(channel);
    char_buffer_put(buf, c);
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
int setfifo(int channel, int state) {
  int *line, buf;
  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      line = (int *) (UART0_BASE + UART_LCRH_OFFSET);
#else
      line = (int *) (UART1_BASE + UART_LCRH_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      line = (int *) (UART1_BASE + UART_LCRH_OFFSET);
#else
      line = (int *) (UART2_BASE + UART_LCRH_OFFSET);
#endif
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

int canputc(int channel) {
  assert(channel == TRAIN || channel == TERMINAL);
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
  }
  if (TRAIN == channel) {
    return !(*flags & (TXFF_MASK | TXBUSY_MASK) && (*flags & CTS_MASK));
  }
  return !(*flags & (TXFF_MASK | TXBUSY_MASK));
}

int cangetc(int channel) {
  assert(channel == TRAIN || channel == TERMINAL);
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
      return -1;
  }
  return *flags & RXFF_MASK;
}

int putc(int channel, char c) {
  volatile int *flags, *data; // just to be safe
  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      flags = (int *) (UART0_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART0_BASE + UART_DATA_OFFSET);
#else
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
#else
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART2_BASE + UART_DATA_OFFSET);
#endif
      break;
    default:
      return -1;
  }
  // TXFF: Transmit buffer full
  if (TRAIN == channel) {
#if VERSATILEPB
    while ((*flags & TXFF_MASK));
#else
    while ((*flags & TXFF_MASK) || !(*flags & CTS_MASK));
#endif
  } else {
    while ((*flags & TXFF_MASK));
  }
  *data = c;
  return 0;
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

char c2x(char ch) {
  if ((ch <= 9)) {
    return '0' + ch;
  } else {
    return 'a' + ch - 10;
  }
}

int putx(int (*put)(char), char c) {
  char chh, chl;

  chh = c2x(c / 16);
  chl = c2x(c % 16);
  put(chh);
  return put(chl);
}

int putr(int (*put)(char), unsigned int reg) {
  int byte;
  char *ch = (char *) &reg;

  for (byte = 3; byte >= 0; byte--) {
    putx(put, ch[byte]);
  }
  return put(' ');
}

void putw(int (*put)(char), int n, char fc, char *bf) {
  char ch;
  char *p = bf;

  while (*p++ && n > 0) n--;
  while (n-- > 0) put(fc);
  while ((ch = *bf++)) put(ch);
}

uint16_t last_err = 0;
bool got_err = false;

char get_error(int channel) {
  assert(channel == TRAIN || channel == TERMINAL);
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
  ERROR_REG = (int *)(ERROR_REG + UART_RSR_OFFSET);
  if (*ERROR_REG) {
    *ERROR_REG = 0; // RESET
  }
  return *ERROR_REG;
#endif
}

int getc(int channel) {
  volatile int *data;
  volatile unsigned char c;

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
  if (channel == TRAIN) { // TODO make versatilepb compatible
    char err = get_error(channel);
    if (err & (OE_MASK | BE_MASK | PE_MASK | FE_MASK)) {
#if DEBUG
      printf("\033[?25h\033[2;1H\033[K");
      printf("OE: %d, BE: %d, PE: %d, FE: %d", err & OE_MASK, err & BE_MASK, err & PE_MASK, err & FE_MASK);
#endif
      oe_in_sensor = 1;
      last_err = get_time();
      got_err = true;
    }
    if (got_err && last_err + (uint16_t)500 < (uint16_t)get_time()) {
#if DEBUG
      printf("\033[?25h\033[2;1H\033[K");
#endif
      got_err = false;
    }
  }
  c = *data & DATA_MASK;
  return c;
}

int bwgetc( int channel ) {
  volatile int *data;
  volatile int *flags;
  volatile unsigned char c;

  switch (channel) {
    case TRAIN:
#if VERSATILEPB
      flags = (int *) (UART0_BASE + UART_FLAG_OFFSET);
      data = (int *) UART0_BASE + UART_DATA_OFFSET;
#else
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART1_BASE + UART_DATA_OFFSET);
#endif
      break;
    case TERMINAL:
#if VERSATILEPB
      flags = (int *) (UART1_BASE + UART_FLAG_OFFSET);
      data = (int *) UART1_BASE + UART_DATA_OFFSET;
#else
      flags = (int *) (UART2_BASE + UART_FLAG_OFFSET);
      data = (int *) (UART2_BASE + UART_DATA_OFFSET);
#endif
      break;
    default:
      return -1;
  }

  while (!(*flags & RXFF_MASK));
  c = *data;
  return c;
}

int a2d(char ch) {
  if (ch >= '0' && ch <= '9') return ch - '0';
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  return -1;
}

char a2i(char ch, char **src, int base, int *nump) {
  int num, digit;
  char *p;

  p = *src;
  num = 0;
  while ((digit = a2d(ch)) >= 0) {
    if (digit > base) break;
    num = num * base + digit;
    ch = *p++;
  }
  *src = p;
  *nump = num;
  return ch;
}

void ui2a(unsigned int num, unsigned int base, char *bf) {
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

void i2a(int num, char *bf) {
  if (num < 0) {
    num = -num;
    *bf++ = '-';
  }
  ui2a(num, 10, bf);
}

void format(int (*put)(char), char *fmt, va_list va) {
  char bf[12];
  char ch, lz;
  int w;

  while ((ch = *(fmt++))) {
    if (ch != '%')
      put(ch);
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
          ch = a2i(ch, &fmt, 10, &w);
          break;
      }
      switch (ch) {
        case 0:return;
        case 'c':
          put(va_arg(va, char));
          break;
        case 's':
          putw(put, w, 0, va_arg(va, char *));
          break;
        case 'u':
          ui2a(va_arg(va, unsigned int), 10, bf);
          putw(put, w, lz, bf);
          break;
        case 'd':
          i2a(va_arg(va, int), bf);
          putw(put, w, lz, bf);
          break;
        case 'x':
          ui2a(va_arg(va, unsigned int), 16, bf);
          putw(put, w, lz, bf);
          break;
        case '%':
          put(ch);
          break;
      }
    }
  }
}

void printf(char *fmt, ...) {
  va_list va;

  va_start(va, fmt);
  format(terminalputc, fmt, va);
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
  format(logputc, fmt, va);
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
