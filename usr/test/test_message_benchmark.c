#include "test_message_benchmark.h"

#define NUM_MSG 1000

/**
 * Offsets:
 * 00 kEntry_sys_send
 * 04 before_receive
 * 08 kEntry_sys_receive
 * 0C kExit_sys_receive
 * 10 after_receive
 * 14 kEntry_sys_reply
 * 18 kExit_sys_send
 * 1C before_copy
 * 20 after_copy
 *
 * Send first: 
 * before_entry                                   |
 * Send()                                         |
 * kEntry (sys_send)                              |
 * -- handle Send(), schedule next task #######   |
 * before_receive                                 |
 * Receive()                                      |
 * kEntry (sys_receive)                           |
 * -- handle Receive(), schedule same task again  |
 * kExit (sys_receive)                            |
 * after_receive/before_reply                     |
 * kEntry (sys_reply)                             |
 * before_copy                                    |
 * -- handle Reply(), schedule send task again    |
 * after_copy                                     |
 * kExit (sys_send)                               |
 * after_entry                                    |
 * 
 * Receive first:
 * before_receive
 * Receive()
 * kEntry (sys_receive)
 * -- handle Receive(), block, schedule next task #####
 * before_entry
 * Send()
 * kEntry(sys_send)
 * -- handle Send(), copy message, schedule receiver
 * kExit(sys_receive)
 * after_receive/before_reply
 * kEntry(sys_reply)
 * before_copy
 * -- handle Reply(), schedule sender
 * after_copy (before schedule)
 * kExit(sys_send)
 * after_entry
 */
void benchmark_reply() {
  int tid;
  char msg[64];
  char msglen = 64;
  volatile int16_t *loc_after_receive = (int16_t*)0x01a0010;
  volatile int16_t *loc_before_receive = (int16_t*)0x01a0004;
  for (int i = 0; i < NUM_MSG; i++) {
    *loc_before_receive = (int16_t)get_clockticks();
    int bytes_received = Receive(&tid, msg, msglen);
    *loc_after_receive = (int16_t)get_clockticks();
    Assert(bytes_received >= 0);
    Reply(tid, msg, bytes_received);
  }
}

int16_t get_avg(int16_t a[NUM_MSG]) {
  int32_t acc = 0;
  for (int i = 0; i < NUM_MSG; i++) {
    acc += a[i];
  }
  return acc / NUM_MSG;
}

int16_t get_worst(int16_t a[NUM_MSG]) {
  int16_t worst = 0;
  for (int i = 0; i < NUM_MSG; i++) {
    if (worst < a[i]) worst = a[i];
  }
  return worst;
}

int16_t get_worst_index(int16_t a[NUM_MSG]) {
  int16_t worst = 0;
  int16_t worst_index = 0;
  for (int i = 0; i < NUM_MSG; i++) {
    if (worst < a[i]) {
      worst = a[i];
      worst_index = i;
    }
  }
  return worst_index;
}

int16_t get_variance(int16_t a[NUM_MSG]) {
  int32_t acc = 0;
  int16_t avg = get_avg(a);
  for (int i = 0; i < NUM_MSG; i++) {
    acc += (a[i] - avg) * (a[i] - avg);
  }
  return acc / (NUM_MSG - 1.0);
}

struct measurement {
  int16_t data[NUM_MSG];
  char name[32];
  int16_t avg, worst, worst_index, var;
};

/*void zip(void *arg1, void *arg2, void *res, int16_t size, (void)(void, void, void)()fn) {
  
  }*/

/**
 * Applies a subtract operation to every element the first two arguments and saves the result in the third.
 * @param arg1 First argument.
 * @param arg2 Second argument.
 * @param res Result (equivalent to \forall arg1[i], arg2[i], res[i] = arg2[i] - arg1[i])
 */
void zip_subtract(int16_t arg1[NUM_MSG], int16_t arg2[NUM_MSG], int16_t res[NUM_MSG]) {
  bool got_pos = false;
  for (int16_t i = 0; i < NUM_MSG; i++) {
    res[i] = arg2[i] - arg1[i];
    got_pos |= res[i] > 0;
    if (got_pos && res[i] < 0)
      Assert(res[i] >= 0);
  }
}

void process_measurement(struct measurement* m, int16_t data1[NUM_MSG], int16_t data2[NUM_MSG]);
void process_measurement(struct measurement* m, int16_t data1[NUM_MSG], int16_t data2[NUM_MSG]) {
  bwprintf("%s\n\r", m->name);
  zip_subtract(data1, data2, m->data);
  m->avg = get_avg(m->data);
  m->worst = get_worst(m->data);
  m->worst_index = get_worst_index(m->data);
  m->var = get_variance(m->data);
}

#define NUM_TIMEPOINTS (int)11
volatile int16_t *loc_kEntry_sys_send = (int16_t*)0x01a0000;
/*
  volatile int16_t *loc_before_receive = (int16_t*)0x01a0004;
  volatile int16_t *loc_kEntry_sys_receive = (int16_t*)0x01a0008;
  volatile int16_t *loc_kExit_sys_receive = (int16_t*)0x01a000C;
  volatile int16_t *loc_after_receive = (int16_t*)0x01a0010;
  volatile int16_t *loc_kEntry_sys_reply= (int16_t*)0x01a0014;
  volatile int16_t *loc_kExit_sys_send = (int16_t*)0x01a0018;
  volatile int16_t *loc_before_copy = (int16_t*)0x01a001C;
  volatile int16_t *loc_after_copy = (int16_t*)0x01a0020;
  volatile int16_t *loc_before_schedule = (int16_t*)0x01a0024;
  volatile int16_t *loc_after_schedule = (int16_t*)0x01a0028; // TODO put this in lib/benchmark_locations.c
*/

void print_measurement_array(struct measurement *mms) {
  for (int i = 0; i < NUM_TIMEPOINTS - 2; i++) {
    bwprintf("%savg, %sworst, %svar, ", mms[i].name, mms[i].name, mms[i].name);
  }
  bwprintf("%sAvg, %sWorst, %sVar\n\r", mms[NUM_TIMEPOINTS - 2].name, mms[NUM_TIMEPOINTS - 2].name, mms[NUM_TIMEPOINTS - 2].name);
  for (int i = 0; i < NUM_TIMEPOINTS - 2; i++) {
    bwprintf("%d, %d, %d, ", mms[i].avg, mms[i].worst, mms[i].var);
  }
  bwprintf("%d, %d, %d\n\r", mms[NUM_TIMEPOINTS - 2].avg, mms[NUM_TIMEPOINTS - 2].worst, mms[NUM_TIMEPOINTS - 2].var);
}

enum benchmark_timepoints {
  BEFORE_SEND,
  AFTER_SEND,
  BEFORE_COPY,
  AFTER_COPY,
  BEFORE_SCHEDULE,
  AFTER_SCHEDULE,
  KENTRY_SYS_SEND,
  BEFORE_RECEIVE,
  KENTRY_SYS_RECEIVE,
  KEXIT_SYS_RECEIVE,
  AFTER_RECEIVE,
  KENTRY_SYS_REPLY,
  KEXIT_SYS_SEND
};  

void run_benchmark(int msg_size, bool send_first) {
  int reply_tid;
  if (send_first) {
    reply_tid = Create(6, &benchmark_reply); // Send before Reply: Yes
    bwprintf("Send first (%d bytes): \n\r", msg_size);
  } else {
    reply_tid = Create(8, &benchmark_reply); // Send before Reply: No
    bwprintf("Receive first (%d bytes): \n\r", msg_size);
  }
  int16_t stats[NUM_TIMEPOINTS + 2][NUM_MSG];
  struct measurement mms[NUM_TIMEPOINTS - 1];
  char msg[64];
  for (int i = 0; i < NUM_MSG; i++) {
    stats[BEFORE_SEND][i] = (int16_t)get_clockticks();
    Send(reply_tid, msg, msg_size, msg, 64);
    stats[AFTER_SEND][i] = (int16_t)get_clockticks();
    for (int j = 0; j < NUM_TIMEPOINTS; j++) {
      int offset = (j >= NUM_TIMEPOINTS - 4) ? 2 + (j % (NUM_TIMEPOINTS - 4)) : 6 + j;
      stats[offset][i] = (int16_t)*(loc_kEntry_sys_send + 4 * j);
      if (i == 0) {
        bwprintf("i: %d, j: %d, offset: %d\n\r", i, j, offset);
      }
      if (stats[offset][i] < 0) {
        bwprintf("%d, %d\n\r");
        Assert(stats[offset][i] < 0);
      }
    }
  }
  memcpy(mms[0].name, "RTT", 4);
  process_measurement(&mms[0], stats[BEFORE_SEND], stats[AFTER_SEND]);
  memcpy(mms[1].name, "copy", 5);
  process_measurement(&mms[1], stats[BEFORE_COPY], stats[AFTER_COPY]);
  memcpy(mms[2].name, "schedule", 9);
  process_measurement(&mms[2], stats[AFTER_SCHEDULE], stats[BEFORE_SCHEDULE]);
  memcpy(mms[3].name, "sendEntry", 10);
  process_measurement(&mms[3], stats[BEFORE_SEND], stats[KENTRY_SYS_SEND]);
  memcpy(mms[4].name, "receiveEntry", 13);
  process_measurement(&mms[4], stats[BEFORE_RECEIVE], stats[KENTRY_SYS_RECEIVE]);
  if (send_first) {
    memcpy(mms[5].name, "handleReceive", 14);
    process_measurement(&mms[5], stats[KENTRY_SYS_RECEIVE], stats[KEXIT_SYS_RECEIVE]);
  } else {
    memcpy(mms[5].name, "handleSend", 11);
    process_measurement(&mms[5], stats[KENTRY_SYS_SEND], stats[KEXIT_SYS_RECEIVE]);
  }
  memcpy(mms[6].name, "receiveExit", 12);
  process_measurement(&mms[6], stats[KEXIT_SYS_RECEIVE], stats[AFTER_RECEIVE]);
  memcpy(mms[7].name, "replyEntry", 11);
  process_measurement(&mms[7], stats[AFTER_RECEIVE], stats[KENTRY_SYS_REPLY]);
  memcpy(mms[8].name, "handleReply", 12);
  process_measurement(&mms[8], stats[KENTRY_SYS_REPLY], stats[KEXIT_SYS_SEND]);
  memcpy(mms[9].name, "sendExit", 9);
  process_measurement(&mms[9], stats[KEXIT_SYS_SEND], stats[AFTER_SEND]);
  /*
  process_measurement(&mms[0], before_send, after_send, "RTT");
  process_measurement(&sendEntry, before_send, kEntry_sys_send, "sendEntry");
  process_measurement(&receiveEntry, before_receive, kEntry_sys_receive, "receiveEntry");
  process_measurement(&handleReceive, kEntry_sys_receive, kExit_sys_receive, "handleReceive"); // TODO mask if receiver first
  process_measurement(&handleSend, kEntry_sys_send, kExit_sys_receive, "handleSend");
  process_measurement(&receiveExit, kExit_sys_receive, after_receive, "receiveExit"); // TODO mask sender if sender first
  process_measurement(&replyEntry, after_receive, kEntry_sys_reply, "replyEntry");
  process_measurement(&handleReply, kEntry_sys_reply, kExit_sys_send, "handleReply");
  process_measurement(&sendExit, kExit_sys_send, after_send, "sendExit");
  process_measurement(&copy, before_copy, after_copy, "copy");
  */
  print_measurement_array(mms);
}
/*
void run_benchmark_receiver_first(int msg_size) {
  int reply_tid = Create(8, &benchmark_reply); // Send before Reply: Yes
  bwprintf("Receive first: \n\r"); // TODO format for .csv

  int16_t before_send[NUM_MSG], after_send[NUM_MSG], kEntry_sys_send[NUM_MSG], before_receive[NUM_MSG], kEntry_sys_receive[NUM_MSG], kExit_sys_receive[NUM_MSG], after_receive[NUM_MSG], kEntry_sys_reply[NUM_MSG], kExit_sys_send[NUM_MSG], before_copy[NUM_MSG], after_copy[NUM_MSG], before_schedule[NUM_MSG], after_schedule[NUM_MSG];
  struct measurement rtt, sendEntry, receiveEntry, handleSend, receiveExit, replyEntry, handleReply, sendExit, copy, schedule;
  char msg[64];
  volatile int16_t *loc_kEntry_sys_send = (int16_t*)0x01a0000;
  int16_t stats[NUM_TIMEPOINTS][NUM_MSG];
  stats[0] = kEntry_sys_send;
  stats[1] = before_receive;
  stats[2] = kEntry_sys_receive;
  stats[3] = kExit_sys_receive;
  stats[4] = 
    after_receive, kEntry_sys_reply, kExit_sys_send, before_copy, after_copy, before_schedule, after_schedule
  };
  for (int i = 0; i < NUM_MSG; i++) {
    before_send[i] = (int16_t)get_clockticks();
    Send(reply_tid, msg, msg_size, msg, 64);
    after_send[i] = (int16_t)get_clockticks();
    for (int j = 0; j < NUM_TIMEPOINTS; j++) {
      stats[j][i] = (int16_t)*(loc_kEntry_sys_send + 4 * j);
    }
  }
  struct measurement mms[NUM_TIMEPOINTS - 1] = {
    rtt, sendEntry, receiveEntry, receiveExit, handleSend, replyEntry, handleReply, sendExit, copy, schedule
  };
  process_measurement(&rtt, before_send, after_send, "RTT");
  process_measurement(&sendEntry, before_send, kEntry_sys_send, "sendEntry");
  process_measurement(&receiveEntry, before_receive, kEntry_sys_receive, "receiveEntry");
  //process_measurement(&handleReceive, kEntry_sys_receive, kExit_sys_receive, "handleReceive");
  process_measurement(&receiveExit, kExit_sys_receive, after_receive, "receiveExit");
  process_measurement(&handleSend, kEntry_sys_send, kExit_sys_receive, "handleSend");
  process_measurement(&replyEntry, after_receive, kEntry_sys_reply, "replyEntry");
  process_measurement(&handleReply, kEntry_sys_reply, kExit_sys_send, "handleReply");
  process_measurement(&sendExit, kExit_sys_send, after_send, "sendExit");
  process_measurement(&copy, before_copy, after_copy, "copy");
  process_measurement(&schedule, before_schedule, after_schedule, "schedule");
  print_measurement_array(&mms);
}
*/

void run_benchmarks_for_msgsize(int msg_size) {
  run_benchmark(msg_size, false);
  run_benchmark(msg_size, true);
}

void message_benchmark() {
  setup_timer();
  run_benchmarks_for_msgsize(4);
  run_benchmarks_for_msgsize(64);
  EnableCaches(true);
  run_benchmarks_for_msgsize(4);
  run_benchmarks_for_msgsize(64);
}
