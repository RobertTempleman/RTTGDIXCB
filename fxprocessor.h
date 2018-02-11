#include <stdio.h>
#include "OS_interface.h"
#include "compressher_limither.h"
#include "RTT_player.h"


class compressor{
 public:
  compressor(rtt_window &_win, string desc, s32 xx,s32 yy);
  rtt_window &win;

  bool i_am_the_final_limiter_section;

  u32 compressor_mute_index;
  u32 compressor_unique_index;
  u32 compressor_lock_index;
  u32 compressor_bypass_index;
  u32 compressor_threshold_index;
  u32 compressor_ratio_index;
  u32 compressor_attack_index;
  u32 compressor_release_index;
  u32 compressor_softness_index;
  u32 limiter_threshold_index;
  u32 limiter_softness_index;
  u32 compressor_output_index;

  float compressor_threshold_dB_value;
  float compressor_softness_value;
  float compressor_attack_value;
  float compressor_low_release_value;
  float compressor_ratio_value;
  float limiter_softness_value;
  float limiter_threshold_dB_value;
  float compressor_output_value;

  void set_compressor_parameters();

  unique_ptr<compress_her_limit_her> compress_her_output;
};


class fxprocessor{
 public:
  fxprocessor(rtt_window &_win,s32 xx,s32 yy);
  rtt_window &win;
  void setup_fxprocessor(s32 xx,s32 yy);
  void add_output_clips_at_knob(s32 xx,s32 yy);
  void add_a_compressor(s32 xx,s32 yy);
  void add_a_player(s32 xx,s32 yy);
  vector<unique_ptr<compressor>> compressors;
  unique_ptr<RTTplayer> player;
  s32 player_x;
  s32 player_y;

};


