#include "event_broker.h"

void event_broker() {
  Assert(RegisterAs("EventBroker") == 0);

  int my_priority = MyPriority();

  int sender_tid;
  message received;

  int subscribed[MAX_EVENT_TYPE][MAX_TASKS];
  for (int i = 0; i < MAX_EVENT_TYPE; i += 1) {
    for (int j = 0; j < MAX_TASKS; j += 1) {
      subscribed[i][j] = 0;
    }
  }

  int num_subscribed[MAX_EVENT_TYPE];
  for (int i = 0; i < MAX_EVENT_TYPE; i += 1) {
    num_subscribed[i] = 0;
  }

  while (true) {
    Assert(Receive(&sender_tid,
                   &received, sizeof(received)) == sizeof(received));
    switch (received.type) {
      case MESSAGE_PUBLISH: {
        event_type type = received.msg.event.type;
        Assert(type < MAX_EVENT_TYPE);

        event_dispatcher_params params;
        tmemcpy(&params.event, &received.msg.event, sizeof(event));

        for (int i = 0; i < num_subscribed[type]; i += 1) {
          params.tid = subscribed[type][i];
          Init(my_priority - 1, &event_dispatcher, (message_body *)&params);
        }

        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      case MESSAGE_SUBSCRIBE: {
        event_type type = received.msg.event.type;
        Assert(type < MAX_EVENT_TYPE);

        Assert(num_subscribed[type] < MAX_TASKS);
        subscribed[type][num_subscribed[type]] = sender_tid;
        num_subscribed[type] += 1;

        Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
        break;
      }
      default:
        logprintf("Unknown message type %d in event broker\n\r", received.type);
        Assert(0);
        break;
    }
  }
}
