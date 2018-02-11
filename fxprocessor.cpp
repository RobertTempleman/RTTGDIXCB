#include <stdio.h>
#include "OS_interface.h"
#include "RTT_text_edit_box.h"
#include "main.h"
#include "fxprocessor.h"


float compressor_global_width =COMP_TRANSFER_FN_W+COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_END_X;
float compressor_global_height=COMP_TRANSFER_FN_H+COMP_TRANSFER_FN_START_Y+COMP_TRANSFER_FN_END_Y;



compressor::compressor(rtt_window &_win, string desc, s32 xx,s32 yy):win(_win){
  s32 x=xx+128;
  s32 y=yy+20;
  compressor_threshold_index =win.add_knob2("bitmaps/knobs/compressor_threshold3.bmp" , x, y, 0, 30.0f, 330.0f, "output compressor", "Compressor threshold",     RELOAD_FROM_PRESET);x+=win.panes[compressor_threshold_index]->extra_bitmap->w;
  compressor_ratio_index     =win.add_knob2("bitmaps/knobs/compressor_ratio72.bmp"    , x, y, 0, 30.0f, 330.0f, "output compressor", "Compressor ratio",         RELOAD_FROM_PRESET);x+=win.panes[compressor_ratio_index    ]->extra_bitmap->w;
  compressor_attack_index    =win.add_knob2("bitmaps/knobs/compressor_attack.bmp"     , x, y, 0, 30.0f, 330.0f, "output compressor", "compressor attack",        RELOAD_FROM_PRESET);x+=win.panes[compressor_attack_index   ]->extra_bitmap->w;
  compressor_release_index   =win.add_knob2("bitmaps/knobs/compressor_release.bmp"    , x, y, 0, 30.0f, 330.0f, "output compressor", "compressor release",       RELOAD_FROM_PRESET);x+=win.panes[compressor_release_index  ]->extra_bitmap->w;
  compressor_softness_index  =win.add_knob2("bitmaps/knobs/compressor_sharpness72.bmp", x, y, 0, 30.0f, 330.0f, "output compressor", "Compressor knee softness", RELOAD_FROM_PRESET);x+=win.panes[compressor_softness_index ]->extra_bitmap->w;
  limiter_threshold_index    =win.add_knob2("bitmaps/knobs/limiter_threshold3.bmp"    , x, y, 0, 30.0f, 330.0f, "output compressor", "Limiter threshold",        RELOAD_FROM_PRESET);x+=win.panes[limiter_threshold_index   ]->extra_bitmap->w;
  limiter_softness_index     =win.add_knob2("bitmaps/knobs/compressor_sharpness72.bmp", x, y, 0, 30.0f, 330.0f, "output compressor", "Limiter knee softness",    RELOAD_FROM_PRESET);x+=win.panes[limiter_softness_index    ]->extra_bitmap->w;
  compressor_output_index    =win.add_knob2("bitmaps/knobs/compressor_output.bmp"     , x, y, 0, 30.0f, 330.0f, "output compressor", "compressor output",        RELOAD_FROM_PRESET);x+=win.panes[compressor_output_index   ]->extra_bitmap->w;
  s32 knobs_end_x=x;
  s32 knobs_width_x=x-xx;

  x=x+128;
  y=yy;
  compressor_mute_index      =win.add_switch2("bitmaps/knobs/compressor_mute5.bmp"  ,x,y, "output compressor", "mute output compressor",   IS_TOGGLE_SWITCH, IS_FLASHING_SWITCH,    DONT_RELOAD_FROM_PRESET);x+=win.panes[compressor_mute_index  ]->extra_bitmap->w;
  compressor_unique_index    =win.add_switch2("bitmaps/knobs/compressor_unique5.bmp",x,y, "output compressor", "unique output",            IS_TOGGLE_SWITCH, IS_FLASHING_SWITCH,    DONT_RELOAD_FROM_PRESET);x+=win.panes[compressor_unique_index]->extra_bitmap->w;
  compressor_lock_index      =win.add_switch2("bitmaps/knobs/compressor_lock5.bmp"  ,x,y, "output compressor", "lock output compressor",   IS_TOGGLE_SWITCH, IS_NONFLASHING_SWITCH, DONT_RELOAD_FROM_PRESET);x+=win.panes[compressor_lock_index  ]->extra_bitmap->w;
  compressor_bypass_index    =win.add_switch2("bitmaps/knobs/compressor_bypass5.bmp",x,y, "output compressor", "bypass output compressor", IS_TOGGLE_SWITCH, IS_FLASHING_SWITCH,    DONT_RELOAD_FROM_PRESET);

  RECT compress_her_LED_rect=RECT(x, y, x+COMPRESSOR_LED_DISPLAY_WIDTH, y+COMPRESSOR_LED_DISPLAY_HEIGHT);

  compress_her_output=unique_ptr<compress_her_limit_her>(new compress_her_limit_her(dynamic_cast<RTTXCB&>(_win), desc.data(), 0.0f, 0.0f ,(float)compressor_global_width, (float)compressor_global_height, compress_her_LED_rect));
  rtt_pane &comp_pane=win.add_pane("compressor graph", NULL, NULL, NULL, xx ,yy, compressor_global_width, compressor_global_height);
  comp_pane.pane_special_functionality=PANE_SPECIAL_FUNCTION_PAINT_FUNCTION_IS_GENERIC_STD_FUNCTION_TO_CALL;
  comp_pane.generic_paint_func=std::bind(&compress_her_limit_her::draw, *compress_her_output, std::ref(comp_pane));

  i_am_the_final_limiter_section=false;
  //  set_compressor_parameters();
}


void compressor::set_compressor_parameters(){
  s32 compressor_threshold_frame=win.panes[compressor_threshold_index]->knob_frame_number;
  s32 compressor_softness_frame =win.panes[compressor_softness_index ]->knob_frame_number;
  s32 compressor_ratio_frame    =win.panes[compressor_ratio_index    ]->knob_frame_number;
  s32 compressor_attack_frame   =win.panes[compressor_attack_index   ]->knob_frame_number;
  s32 compressor_release_frame  =win.panes[compressor_release_index  ]->knob_frame_number;
  s32 limiter_threshold_frame   =win.panes[limiter_threshold_index   ]->knob_frame_number;
  s32 limiter_softness_frame    =win.panes[limiter_softness_index    ]->knob_frame_number;
  s32 compressor_output_frame   =win.panes[compressor_output_index   ]->knob_frame_number;

  float compressor_threshold_normalized=win.panes[compressor_threshold_index]->knob_float_to_change_to_reflect_current_setting;
  float limiter_threshold_normalized   =win.panes[limiter_threshold_index]->knob_float_to_change_to_reflect_current_setting;
  float compressor_output_normalized   =win.panes[compressor_output_index]->knob_float_to_change_to_reflect_current_setting;
    
  compressor_threshold_dB_value=-40.0f+60.0f*compressor_threshold_normalized;
  compressor_softness_value    =compress_her_limit_her::compressor_knee_radius[compressor_softness_frame];
  compressor_ratio_value       =compress_her_limit_her::ratios[compressor_ratio_frame];
  compressor_attack_value      =compress_her_limit_her::attack_time[compressor_attack_frame];
  compressor_low_release_value =compress_her_limit_her::release_time[compressor_release_frame];
  limiter_threshold_dB_value   =-20.0f+40.0f*limiter_threshold_normalized;
  limiter_softness_value       =compress_her_limit_her::compressor_knee_radius[limiter_softness_frame];
  compressor_output_value      =compressor_output_normalized*20.0f-5.0f;
  
  compress_her_output->set_global_parameters_low(compressor_threshold_dB_value,
                                                 compressor_softness_value,
                                                 compressor_ratio_value,
                                                 compressor_attack_value,
                                                 compressor_low_release_value,
                                                 limiter_threshold_dB_value,
                                                 limiter_softness_value,
                                                 compressor_output_value,
                                                 i_am_the_final_limiter_section);

}


fxprocessor::fxprocessor(rtt_window &_win,s32 xx,s32 yy):win(_win){
  setup_fxprocessor(xx,yy);
}


void fxprocessor::setup_fxprocessor(s32 xx,s32 yy){
  add_output_clips_at_knob(xx,yy);
  add_a_compressor(xx,yy+250);
  add_a_player(xx,yy+440);
}


void fxprocessor::add_output_clips_at_knob(s32 xx,s32 yy){
  win.add_knob2("bitmaps/knobs/clips_at_27_2.bmp", 10 ,10, 0, 60.0f, 300.0f, "Output", "output scaling", DONT_RELOAD_FROM_PRESET);
}


void fxprocessor::add_a_compressor(s32 xx,s32 yy){
  compressors.push_back(unique_ptr<compressor>(new compressor(win,"Out compressor",xx,yy)));
  //  compressors.push_back(unique_ptr<compressor>(new compressor(win,"Out compressor",xx,yy+92)));
}


void fxprocessor::add_a_player(s32 xx,s32 yy){
  player=unique_ptr<RTTplayer>(new RTTplayer());
  player_x=xx;
  player_y=yy;
}
