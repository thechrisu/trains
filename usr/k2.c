#include "k2.h"

#define TASKS 64

unsigned int prng_seed = 0xBEA71E5;

unsigned int prng_next() {
  prng_seed = prng_seed * 1103515245 + 12345;
  return prng_seed;
}

int outcome(char throw1, char throw2) {
  if (throw1 == throw2) {
    return 0;
  } else if ((throw1 == 'R' && throw2 == 'S') || (throw1 == 'P' && throw2 == 'R') || (throw1 == 'S' && throw2 == 'P')) {
    return 1;
  } else if ((throw1 == 'R' && throw2 == 'P') || (throw1 == 'P' && throw2 == 'S') || (throw1 == 'S' && throw2 == 'R')) {
    return -1;
  } else {
    return 0xFADED0DE;
  }
}

void k2_rps_server() {
  int partners[TASKS];
  char throws[TASKS];
  int pending_clients[2];
  int pending_clients_index = 0;

  int sender_tid;
  char received;

  for (int i = 0; i < TASKS; i += 1) {
    partners[i] = 0;
    throws[i] = 0;
  }

  RegisterAs("RPS");

  while (true) {
    Receive(&sender_tid, &received, 1);
    int partner = partners[sender_tid];

    switch (received) {
      case 'B':
        if (partner) {
          bwprintf("Task %d tried to sign up when it already has a partner (task %d)\n\r", sender_tid, partner);
          Reply(sender_tid, "N", 1);
        } else {
          bwprintf("Task %d signed up to play\n\r", sender_tid);
          pending_clients[pending_clients_index] = sender_tid;
          pending_clients_index += 1;

          if (pending_clients_index == 2) {
            bwprintf("Matched tasks %d and %d\n\r", pending_clients[0], pending_clients[1]);
            pending_clients_index = 0;
            partners[pending_clients[0]] = pending_clients[1];
            partners[pending_clients[1]] = pending_clients[0];
            Reply(pending_clients[0], "Y", 1);
            Reply(pending_clients[1], "Y", 1);
          }
        }
        break;
      case 'R':
      case 'P':
      case 'S':
        if (!partner) {
          bwprintf("Task %d tried to play when it doesn't have a partner\n\r", sender_tid);
          Reply(sender_tid, "N", 1);
        } else {
          bwprintf("Task %d played %c\n\r", sender_tid, received);
          if (throws[partner]) {
            switch (outcome(received, throws[partner])) {
              case 1:
                bwprintf("Task %d wins with %c, task %d loses with %c\n\r", sender_tid, received, partner, throws[partner]);
                bwgetc(TERMINAL);
                Reply(sender_tid, "W", 1);
                Reply(partner, "L", 1);
                break;
              case 0:
                bwprintf("Tasks %d and %d draw with %c\n\r", sender_tid, partner, received);
                bwgetc(TERMINAL);
                Reply(sender_tid, "D", 1);
                Reply(partner, "D", 1);
                break;
              case -1:
                bwprintf("Task %d wins with %c, task %d loses with %c\n\r", partner, throws[partner], sender_tid, received);
                bwgetc(TERMINAL);
                Reply(sender_tid, "L", 1);
                Reply(partner, "W", 1);
                break;
              default:
                bwprintf("Invalid game between tasks %d (threw %c) and %d (threw %c)\n\r", sender_tid, received, partner, throws[partner]);
                bwgetc(TERMINAL);
                Reply(sender_tid, "N", 1);
                Reply(partner, "N", 1);
                break;
            }
            throws[partner] = 0;
          } else {
            throws[sender_tid] = received;
          }
        }
        break;
      case 'Q':
        if (!partner) {
          bwprintf("Task %d tried to quit when it doesn't have a partner\n\r", sender_tid);
          Reply(sender_tid, "N", 1);
        } else {
          bwprintf("Task %d quit\n\r", sender_tid);
          throws[sender_tid] = 0;
          throws[partner] = 0;
          partners[sender_tid] = 0;
          partners[partner] = 0;
          Reply(sender_tid, "Y", 1);
          Reply(partner, "N", 1);
        }
        break;
    }
  }
}

char send_to_rps_server(char c) {
  int rps_server_tid = WhoIs("RPS");
  char received;

  Send(rps_server_tid, &c, 1, &received, 1);
  return received;
}

char signup() {
  return send_to_rps_server('B');
}

char play(char throw) {
  return send_to_rps_server(throw);
}

char quit() {
  return send_to_rps_server('Q');
}

void play_games() {
  char throws[] = { 'R', 'P', 'S' };
  int my_tid = MyTid();

  signup();

  for (int i = 0; i < (my_tid ^ 0x1); i += 1) {
    char result = play(throws[prng_next() % 3]);
    if (result == 'N') {
      return;
    }
  }

  quit();
}

void k2_rps_client() {
  play_games();

  if (MyTid() % 2 == 0) {
    play_games();
  }

  bwprintf("Task %d exiting\n\r", MyTid());
}

void k2_first_user_task() {
  Create(0, &nameserver_main);
  Create(0, &k2_rps_server);

  for (int i = 0; i < 4; i += 1) {
    Create(0, &k2_rps_client);
  }
}
