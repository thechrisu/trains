#include "switcher.h"

#include "track_data.h"

#define DELAY_BETWEEN_SW 50

void switcher() {
  int sender_tid;
  message received;

  Assert(Receive(&sender_tid, &received, sizeof(received)) == sizeof(received));
  Assert(sender_tid == MyParentTid());
  int turnout_num = received.msg.switch_params.turnout_num;
  bool curved = received.msg.switch_params.curved;
#if DEBUG_SWITCHING
  logprintf("will: %s to %s\n\r", turnout_num_to_node(&track, turnout_num)->name, curved ? "Curved" : "Straight");
#endif /* DEBUG_SWITCHING */
  Assert(Reply(sender_tid, EMPTY_MESSAGE, 0) == 0);
  Assert(received.type == MESSAGE_SWITCH);

  int clock_server_tid = received.msg.switch_params.clock_server_tid;
  int tx_server_tid = received.msg.switch_params.tx_server_tid;
  int track_state_controller_tid = WhoIs("TrackStateController");
#if DEBUG_SWITCHING
  logprintf("will: %s to %s\n\r", turnout_num_to_node(&track, turnout_num)->name, curved ? "Curved" : "Straight");
#endif /* DEBUG_SWITCHING */

  volatile int t = Time(clock_server_tid);
  Assert(t > 0);
  Assert(turnout_num_to_map_offset(turnout_num) < NUM_TURNOUTS);
  if (t >= track.last_switch_time[turnout_num_to_map_offset(turnout_num)] + DELAY_BETWEEN_SW) {
    track.last_switch_time[turnout_num_to_map_offset(turnout_num)] = t;
#if DEBUG_SWITCHING
    logprintf("Switching for realzies: %s to %s (%d)\n\r", turnout_num_to_node(&track, turnout_num)->name, curved ? "Curved" : "Straight", t);
#endif /* DEBUG_SWITCHING */
    switch_turnout(clock_server_tid, tx_server_tid, track_state_controller_tid, turnout_num, curved);
  }
}
