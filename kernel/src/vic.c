#include "vic.h"


register_t event_masks[MAX_EVENT_ID + 1];

int event_id_to_vic_bit(enum event_id e) {
#if VERSATILEPB
  switch (e) {
    case TIMER_INTERRUPT:
      return 5;
    case TRAIN_TX_INTERRUPT:
    case TRAIN_RX_INTERRUPT:
      return 12;
    case TERMINAL_RX_INTERRUPT:
    case TERMINAL_TX_INTERRUPT:
      return 13;
    default:
      return -1;
  }
#else
  switch (e) {
    case TIMER_INTERRUPT:
      return 4;
    case TRAIN_RX_INTERRUPT:
      return 23;
    case TRAIN_TX_INTERRUPT:
      return 24;
    case TERMINAL_RX_INTERRUPT:
      return 25;
    case TERMINAL_TX_INTERRUPT:
      return 26;
    default:
      return -1;
  }
#endif /* VERSATILEPB */
}

int get_highest_vic_bit_event_id(register_t pic_status) {
  for (int i = 0; i <= MAX_EVENT_ID; i++) {
    if (pic_status & event_masks[i]) {
      return i;
    }
  }
  return -1;
}

void enable_vic_bit(int bit) {
#ifndef TESTING
  volatile register_t *vic_reg;
  if (bit == -1) return;
  if (bit < 32) {
    vic_reg = (register_t *)(VIC1_BASE + VIC1_ENABLE_OFFSET);
  } else {
    vic_reg = (register_t *)(VIC2_BASE + VIC2_ENABLE_OFFSET);
    bit -= 32;
  }
  *vic_reg |= 1 << bit;
#endif /* TESTING */
}

void disable_vic_bit(int bit) {
#ifndef TESTING
  volatile register_t *vic_reg;
  if (bit == -1) return;
  if (bit < 32) {
    vic_reg = (register_t *)(VIC1_BASE + VIC1_ENABLE_OFFSET);
  } else {
    vic_reg = (register_t *)(VIC2_BASE + VIC2_ENABLE_OFFSET);
    bit -= 32;
  }
  *vic_reg &= !(1 << bit);
#endif /* TESTING */
}

void enable_in_vic(enum event_id e) {
  enable_vic_bit(event_id_to_vic_bit(e));
}

void disable_in_vic(enum event_id e) {
  disable_vic_bit(event_id_to_vic_bit(e));
}

void clear_in_vic(enum event_id e) {
#ifndef TESTING
  volatile register_t *vic_reg;
  int bit = event_id_to_vic_bit(e);
  if (bit == -1) return;
  if (bit < 32) {
    vic_reg = (register_t *)(VIC1_BASE + VIC1_INTCLR_OFFSET);
  } else {
    vic_reg = (register_t *)(VIC2_BASE + VIC2_INTCLR_OFFSET);
    bit -= 32;
  }
  *vic_reg |= 1 << bit;
#endif /* TESTING */
}

void vic_maskall() {
#ifndef TESTING
  *(uint32_t *)(VIC1_BASE + VIC1_ENABLE_OFFSET) = 0;
  *(uint32_t *)(VIC2_BASE + VIC2_ENABLE_OFFSET) = 0;
  *(uint32_t *)(VIC1_BASE + VIC1_INTCLR_OFFSET) = 0xFFFFFFFF;
  *(uint32_t *)(VIC2_BASE + VIC2_INTCLR_OFFSET) = 0xFFFFFFFF;
#endif /* TESTING */
}

void setup_vic() {
#ifndef TESTING
  vic_maskall();
  enable_in_vic(TIMER_INTERRUPT);
#if !VERSATILEPB || IOINTERRUPTS || !E2ETESTING
  enable_in_vic(TRAIN_TX_INTERRUPT);
  enable_in_vic(TRAIN_RX_INTERRUPT);
  enable_in_vic(TERMINAL_TX_INTERRUPT);
  enable_in_vic(TERMINAL_RX_INTERRUPT);
  enable_in_vic(TERMINAL_RX_INTERRUPT);
  enable_vic_bit(UART1_BIT); // THE TRAIN, CHOO CHOO
#endif /* !VERSATILEPB || IOINTERRUPTS || !E2ETESTING */
#if VERSATILEPB
  event_masks[TERMINAL_TX_INTERRUPT] = VIC1_UART1_MASK;
  event_masks[TERMINAL_RX_INTERRUPT] = VIC1_UART1_MASK;
  event_masks[TRAIN_TX_INTERRUPT] = VIC1_UART0_MASK;
  event_masks[TRAIN_RX_INTERRUPT] = VIC1_UART0_MASK;
#else
  event_masks[TERMINAL_TX_INTERRUPT] = VIC1_UART2TXINT_MASK;
  event_masks[TERMINAL_RX_INTERRUPT] = VIC1_UART2RXINT_MASK;
  event_masks[TRAIN_TX_INTERRUPT] = VIC1_UART1TXINT_MASK;
  event_masks[TRAIN_RX_INTERRUPT] = VIC1_UART1RXINT_MASK;
  /*  TRAIN_TX_INTERRUPT,
      TRAIN_RX_INTERRUPT, */

#endif /* VERSATILEPB */
  event_masks[TIMER_INTERRUPT] = VIC1_TIMER_MASK;
#endif /* TESTING */
}
