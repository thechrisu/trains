#include "test_message_benchmark.h"

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
 * before_entry
 * Send()
 * kEntry (sys_send)
 * -- handle Send(), schedule next task
 * before_receive
 * Receive()
 * kEntry (sys_receive)
 * -- handle Receive(), schedule same task again
 * (before_copy)
 * (after_copy)
 * kExit (sys_receive)
 * after_receive/before_reply
 * kEntry (sys_reply)
 * -- handle Reply(), schedule send task again
 * kExit (sys_send)
 * after_entry
 * 
 * Receive first:
 * before_receive
 * Receive()
 * kEntry (sys_receive)
 * -- handle Receive(), block, schedule next task #####
 * before_entry
 * Send()
 * kEntry(sys_send)
 * before_copy
 * after_copy
 * -- handle Send(), copy message, schedule receiver
 * kExit(sys_receive)
 * after_receive/before_reply
 * kEntry(sys_reply)
 * -- handle Reply(), schedule sender
 * (before receive)
 * (kEntry receive)
 * kExit(sys_send)
 * after_entry
 */
void benchmark_reply() {
  int tid;
  char msg[64];
  char msglen = 64;
  for (int i = 0; i < NUM_MSG; i++) {
    *loc_before_receive = (int16_t) get_clockticks();
    // bwprintf("(%d) Before receive\n\r", *loc_before_receive);
    int bytes_received = Receive(&tid, msg, msglen);
    *loc_after_receive = (int16_t) get_clockticks();
    // bwprintf("(%d) After receive\n\r", *loc_after_receive);
    Assert(bytes_received >= 0);
    Reply(tid, msg, bytes_received);
  }
}

void run_benchmark(int msg_size, bool send_first) {
  int reply_tid;
  if (send_first) {
    reply_tid = Create(6, &benchmark_reply); // Send before Reply: Yes
    bwprintf("Send first (%d bytes): \n\r", msg_size);
  } else {
    reply_tid = Create(8, &benchmark_reply); // Send before Reply: No
    bwprintf("Receive first (%d bytes): \n\r", msg_size);
  }
  *tid_send = MyTid();
  *tid_receive_reply = reply_tid;
  int16_t stats[NUM_TIMEPOINTS + 2][NUM_MSG];
  struct measurement mms[NUM_TIMEPOINTS - 1];
  char msg[64];
  for (int i = 0; i < NUM_MSG; i++) {
    stats[BEFORE_SEND][i] = (int16_t) get_clockticks();
    // bwprintf("(%d) Before send\n\r", stats[BEFORE_SEND][i]);
    Send(reply_tid, msg, msg_size, msg, 64);
    // bwprintf("-------------\n\r");
    stats[AFTER_SEND][i] = (int16_t) get_clockticks();
    // bwprintf("(%d) After send\n\r", stats[AFTER_SEND][i]);
    for (int j = 0; j < NUM_TIMEPOINTS; j++) {
      int offset = (j >= NUM_TIMEPOINTS - 4) ? 2 + (j % (NUM_TIMEPOINTS - 4)) : 6 + j;
      stats[offset][i] = (int16_t) * (loc_kEntry_sys_send + j);
    }
  }
  memcpy(mms[0].name, "RTT", 4);
  process_measurement(&mms[0], stats[BEFORE_SEND], stats[AFTER_SEND]);
  memcpy(mms[1].name, "copy", 5);
  process_measurement(&mms[1], stats[BEFORE_COPY], stats[AFTER_COPY]);
  memcpy(mms[2].name, "schedule", 9);
  process_measurement(&mms[2], stats[BEFORE_SCHEDULE], stats[AFTER_SCHEDULE]);
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
  process_measurement(&mms[8], stats[KENTRY_SYS_REPLY], stats[BEFORE_RECEIVE]);
  memcpy(mms[9].name, "sendExit", 9);
  process_measurement(&mms[9], stats[KEXIT_SYS_SEND], stats[AFTER_SEND]);

  print_measurement_array(mms);
}

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
