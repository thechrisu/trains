#ifndef EVENT_DATA_H
#define EVENT_DATA_H

#define MAX_EVENT_ID 0

enum event_id {
  TIMER_INTERRUPT,
  NOT_BLOCKED = 0xBA5EBA11
};

#endif /* EVENT_DATA_H */
