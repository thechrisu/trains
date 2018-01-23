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
  char msg[64];
  int16_t total_start = get_clockticks();
  for(int i = 0; i < NUM_MSG_PER_SIZE; i++) {
    int16_t start_ticks = (int16_t)get_clockticks();
    Send(reply_task, msg, msg_size, msg, 64);
    int16_t after_sending_4 = (int16_t)get_clockticks();
    rtt_time[i] = after_sending_4 - start_ticks;
  }
  int32_t average_time, worst_time, worst_index, variance, total_time;
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
    }
    average_time += rtt_time[i];
    Assert(average_time > 0);
  }
  average_time /= NUM_MSG_PER_SIZE;
  for (int i = 0; i < NUM_MSG_PER_SIZE; i++) {
    variance += (average_time - rtt_time[i]) * (average_time - rtt_time[i]);
  }
  variance /= (NUM_MSG_PER_SIZE - 1.0);
  bwprintf("%d bytes message RTT (ticks): Avg: %d, Worst: %d (index %d), Variance: %d. Total ticks for %d messages: %d\n\r",
           msg_size, average_time, worst_time, worst_index, variance, NUM_MSG_PER_SIZE, total_time);
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
}
