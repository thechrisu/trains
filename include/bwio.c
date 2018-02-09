#include "bwio.h"

char_buffer train_input_buf, train_output_buf,
  terminal_input_buf, terminal_output_buf;

char train_input_chars[IOBUFFERSZ], train_output_chars[IOBUFFERSZ],
  terminal_input_chars[IOBUFFERSZ], terminal_output_chars[IOBUFFERSZ];

int oe_in_sensor = 0;

void setup_buffered_io() {
  char_buffer_init(&train_input_buf, train_input_chars, IOBUFFERSZ);
  char_buffer_init(&train_output_buf, train_output_chars, IOBUFFERSZ);
  char_buffer_init(&terminal_input_buf, terminal_input_chars, IOBUFFERSZ);
  char_buffer_init(&terminal_output_buf, terminal_output_chars, IOBUFFERSZ);
}

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
  return char_buffer_length(buf);
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
  while (rawcanputc(channel) && !char_buffer_is_empty(buf)) {
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
  while (rawcangetc(channel)) {
    // putc(TERMINAL, cangetc(TERMINAL) ? 'T' : 'F');
    // putc(TRAIN, cangetc(TERMINAL) ? 'T' : 'F');
    if (char_buffer_is_full(buf)) {
      putc(TERMINAL, 'Z');
      putc(TERMINAL, '0' + channel);
      putc(TERMINAL, 'Z');
      putc(TERMINAL, '0' + channel);
      putc(TERMINAL, 'Z');
      putc(TERMINAL, '0' + channel);
      putc(TERMINAL, 'Z');
      dumpbuf(buf);
      return -3;
    }
    char c = rawgetc(channel);
    char_buffer_put(buf, c);
  }
  return 0;
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
    int got_empty = 0;
    int got_cts = 0;
    while (!(got_cts && got_empty)) {
      got_cts |= *flags & CTS_MASK;
      got_empty |= *flags & TXFE_MASK;
    }
#endif
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
