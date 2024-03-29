/**
 * @file
 * @brief Defines the types of events one can wait on.
 */
#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#define MAX_EVENT_ID 4

enum event_id {
  TERMINAL_TX_INTERRUPT,
  TERMINAL_RX_INTERRUPT,
  TRAIN_TX_INTERRUPT,
  TRAIN_RX_INTERRUPT,
  TIMER_INTERRUPT,
  NOT_BLOCKED = 0xBA5EBA11
};

#endif /* EVENT_DATA_H */
