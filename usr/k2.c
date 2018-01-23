/**
 * @file
 * @brief K2 user task implementations: main task, RPS server and clients
 */

#include "k2.h"

#define TASKS 64

unsigned int prng_seed = 0xBEA71E5;

/**
 * Uses a linear congruential generator to generate a pseudorandom number.
 *
 * @returns A pseudorandomly-generated unsigned integer.
 */
unsigned int prng_next() {
  prng_seed = prng_seed * 1103515245 + 12345;
  return prng_seed;
}

/**
 * Returns the outcome of a game of RPS. Each throw should be one of 'R', 'P', or 'S'.
 *
 * @param   throw1 The first throw.
 * @param   throw2 The second throw.
 * @returns 0 if the throws are the same.
 *          1 if throw1 wins.
 *          -1 if throw2 wins.
 *          0xFADED0DE if one or both of the throws is invalid.
 */
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

/**
 * A user task that allows clients to play RPS against each other.
 *
 * The RPS server registers with the nameserver under the name "RPS".
 *
 * All messages sent to the server are expected to be one character long. All replies are
 * also one character long. Three types of messages are accepted:
 *
 * 1. Signup ('B'): Returns 'N' if the task already has a partner, or 'Y' once it
 *    has been matched with a partner.
 * 2. Play ('R', 'P', or 'S'): Returns 'N' if the task doesn't have a partner or if either
 *    the task's or its partner's throw is invalid. Returns 'W' if the task wins, 'L' if
 *    it loses, and 'D' if there is a draw.
 * 3. Quit ('Q'): Returns 'N' if the task doesn't have a partner, or 'Y' otherwise.
 */
void k2_rps_server() {
  int partners[TASKS];
  char throws[TASKS];
  int pending_clients[2];
  int pending_clients_index = 0;
  char replies[2];

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
                replies[0] = 'W';
                replies[1] = 'L';
                break;
              case 0:
                bwprintf("Tasks %d and %d draw with %c\n\r", sender_tid, partner, received);
                replies[0] = 'D';
                replies[1] = 'D';
                break;
              case -1:
                bwprintf("Task %d wins with %c, task %d loses with %c\n\r", partner, throws[partner], sender_tid, received);
                replies[0] = 'L';
                replies[1] = 'W';
                break;
              default:
                bwprintf("Invalid game between tasks %d (threw %c) and %d (threw %c)\n\r", sender_tid, received, partner, throws[partner]);
                replies[0] = 'N';
                replies[1] = 'N';
                break;
            }
#ifndef E2ETESTING
            bwgetc(TERMINAL);
#endif
            Reply(sender_tid, &(replies[0]), 1);
            Reply(partner, &(replies[1]), 1);
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

/**
 * Sends a one-character message to the RPS server.
 *
 * @param   c The character to send.
 * @returns The one-character response from the RPS server.
 */
char send_to_rps_server(char c) {
  int rps_server_tid = WhoIs("RPS");
  char received;

  Send(rps_server_tid, &c, 1, &received, 1);
  return received;
}

/**
 * Signs the current task up with the RPS server.
 *
 * @returns The one-character response from the RPS server.
 */
char signup() {
  return send_to_rps_server('B');
}

/**
 * Makes a play in a game of RPS.
 *
 * @param   throw One of 'R', 'P', or 'S'.
 * @returns The one-character response from the RPS server.
 */
char play(char throw) {
  return send_to_rps_server(throw);
}

/**
 * Tells the RPS server that the current task wants to quit.
 *
 * @returns The one-character response from the RPS server.
 */
char quit() {
  return send_to_rps_server('Q');
}

/**
 * Plays a series of RPS games.
 */
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

/**
 * Plays a series of RPS games, then plays another series if MyTid() is even.
 */
void k2_rps_client() {
  play_games();

  if (MyTid() % 2 == 0) {
    play_games();
  }

  bwprintf("Task %d exiting\n\r", MyTid());
}

/**
 * Starts the nameserver and RPS server, then initializes a number of RPS clients.
 */
void k2_first_user_task() {
  Create(5, &nameserver_main);
  Create(5, &k2_rps_server);

  for (int i = 0; i < 4; i += 1) {
    Create(5, &k2_rps_client);
  }
}
