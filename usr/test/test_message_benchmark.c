#include "test_message_benchmark.h"

#define NUM_MSG_PER_SIZE 10000

void benchmark_reply() {
  int tid;
  char msg[64];
  char msglen = 64;
  for (int i = 0; i < NUM_MSG_PER_SIZE; i++) {
    int bytes_received = Receive(&tid, msg, msglen);
    Assert(bytes_received >= 0);
    Reply(tid, msg, bytes_received);
  }
}

void run_benchmark(int msg_size, int reply_task) {
  int16_t rtt_time[NUM_MSG_PER_SIZE];
#if CONTEXT_SWITCH_BENCHMARK
  int16_t kEntry_time[NUM_MSG_PER_SIZE];
  int16_t kHandle_time[NUM_MSG_PER_SIZE];
  int16_t kExit_time[NUM_MSG_PER_SIZE];
  volatile int16_t* after_kentry = (int16_t*)0x01a00000;
  volatile int16_t* before_kexit = (int16_t*)0x01a00004;
#endif /* CONTEXT_SWITCH_BENCHMARK */
  char msg[64];
  int16_t total_start = get_clockticks();
  for(int i = 0; i < NUM_MSG_PER_SIZE; i++) {
    int16_t start_ticks = (int16_t)get_clockticks();
    Send(reply_task, msg, msg_size, msg, 64);
    int16_t after_sending = (int16_t)get_clockticks();
#if CONTEXT_SWITCH_BENCHMARK
    kEntry_time[i] = (*after_kentry) - start_ticks;
    Assert(kEntry_time[i] < 10000000);
    kHandle_time[i] = (*before_kexit) - (*after_kentry);
    kExit_time[i] = after_sending - (*before_kexit);
    Assert(kEntry_time[i] > 0);
    Assert(kHandle_time[i] > 0);
    // Assert(kExit_time[i] > 0);
#endif /* CONTEXT_SWITCH_BENCHMARK */
    rtt_time[i] = after_sending - start_ticks;
    Assert(rtt_time[i] > 0);
  }
  int16_t worst_time, worst_index, total_time;
  int16_t average_time, variance;
#if CONTEXT_SWITCH_BENCHMARK
  int16_t kEntryAvg, kEntryWorst, kEntryVariance;
  int16_t kHandleAvg, kHandleWorst, kHandleVariance;
  int16_t kExitAvg, kExitWorst, kExitVariance;
  kEntryAvg = 0;
  kEntryWorst = 0;
  kEntryVariance = 0;
  kHandleAvg = 0;
  kHandleWorst = 0;
  kHandleVariance = 0;
  kExitAvg = 0;
  kExitWorst = 0;
  kExitVariance = 0;
#endif /* CONTEXT_SWITCH_BENCHMARK */
  average_time = 0;
  worst_time = 0;
  worst_index = 0;
  variance = 0;
  total_time = get_clockticks() - total_start;
  for (int i = 0; i < NUM_MSG_PER_SIZE; i++) {
    if (rtt_time[i] > worst_time) {
      bwprintf("Worst rtt: %d\n\r", rtt_time[i]);
      worst_time = rtt_time[i];
      worst_index = i;
#if CONTEXT_SWITCH_BENCHMARK
      if (kEntry_time[i] > kEntryWorst)
        kEntryWorst = kEntry_time[i];
      if (kHandle_time[i] > kHandleWorst)
        kHandleWorst = kHandle_time[i];
      if (kExit_time[i] > kExitWorst)
        kExitWorst = kExit_time[i];
#endif /* CONTEXT_SWITCH_BENCHMARK */
    }
    average_time += rtt_time[i];
#if CONTEXT_SWITCH_BENCHMARK
    kEntryAvg += kEntry_time[i];
    kHandleAvg += kHandle_time[i];
    kExitAvg += kExit_time[i];
#endif /* CONTEXT_SWITCH_BENCHMARK */
    Assert(average_time > 0);
  }
  average_time /= NUM_MSG_PER_SIZE;
#if CONTEXT_SWITCH_BENCHMARK
  kEntryAvg /= NUM_MSG_PER_SIZE;
  kHandleAvg /= NUM_MSG_PER_SIZE;
  kExitAvg /= NUM_MSG_PER_SIZE;
#endif /* CONTEXT_SWITCH_BENCHMARK */
  for (int i = 0; i < NUM_MSG_PER_SIZE; i++) {
    int32_t this_run = (average_time - rtt_time[i]) * (average_time - rtt_time[i]);
#if CONTEXT_SWITCH_BENCHMARK
    int32_t kEntry_run = (kEntryAvg - kEntry_time[i]) * (kEntryAvg - kEntry_time[i]);
    int32_t kHandle_run = (kHandleAvg - kHandle_time[i]) * (kHandleAvg - kHandle_time[i]);
    int32_t kExit_run = (kExitAvg - kExit_time[i]) * (kExitAvg - kExit_time[i]);
#endif /* CONTEXT_SWITCH_BENCHMARK */
    if (this_run < 0) {// sometimes happens, although it really shouldn't, by the laws of multiplication.
      this_run *= -1;
      bwprintf("%d, %d\n\r", average_time, rtt_time[i]);
    }
    if (rtt_time[i] == 0) {
      bwprintf("rtt ==0 for index %d\n\r", i);
    }
    //bwprintf("variance: %d, this run: %d, this rtt: %d, index: %d\n\r", (int16_t)variance, (int16_t)this_run, (int16_t)rtt_time[i], i);
    // Assert(this_run > 0);
    variance += this_run;
#if CONTEXT_SWITCH_BENCHMARK
    kEntryVariance += kEntry_run;
    kHandleVariance += kHandle_run;
    kExitVariance += kExit_run;
#endif /* CONTEXT_SWITCH_BENCHMARK */
    // Assert((variance > 0));
  }
  variance /= (NUM_MSG_PER_SIZE - 1.0);
#if CONTEXT_SWITCH_BENCHMARK
  kEntryVariance /= (NUM_MSG_PER_SIZE - 1.0);
  kHandleVariance /= (NUM_MSG_PER_SIZE - 1.0);
  kExitVariance /= (NUM_MSG_PER_SIZE - 1.0);
#endif /* CONTEXT_SWITCH_BENCHMARK */
  bwprintf("%d bytes message RTT (ticks): Avg: %d, Worst: %d (index %d), Variance: %d. Total ticks for %d messages: %d\n\r",
           msg_size, (int16_t)average_time, worst_time, worst_index, (int16_t)variance, NUM_MSG_PER_SIZE, total_time);
#if CONTEXT_SWITCH_BENCHMARK
  bwprintf("kEntry avg: %d, worst: %d, variance: %d\n\r", kEntryAvg, kEntryWorst, kEntryVariance);
  bwprintf("kHandle avg: %d, worst: %d, variance: %d\n\r", kHandleAvg, kHandleWorst, kHandleVariance);
  bwprintf("kExit avg: %d, worst: %d, variance: %d\n\r", kExitAvg, kExitWorst, kExitVariance);
#endif /* CONTEXT_SWITCH_BENCHMARK */
}

void run_benchmarks_for_msgsize(int msg_size) {
  int reply_task_receive_first = Create(8, &benchmark_reply); // Send before Reply: No
  bwprintf("Receive first: \n\r");
  run_benchmark(msg_size, reply_task_receive_first);
  int reply_task_send_first = Create(6, &benchmark_reply); // Send before Reply: Yes
  bwprintf("Send first: \n\r");
  run_benchmark(msg_size, reply_task_send_first);
}

void message_benchmark() {
  setup_timer();
  run_benchmarks_for_msgsize(4);
  run_benchmarks_for_msgsize(64);
  EnableCaches(true);
  run_benchmarks_for_msgsize(4);
  run_benchmarks_for_msgsize(64);
}
