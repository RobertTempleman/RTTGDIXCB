#include <stdio.h>
#include "OS_interface.h"
#include "RTT_text_edit_box.h"
#include "main.h"
#include "fxprocessor.h"


float compressor_global_width =COMP_TRANSFER_FN_W+COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_END_X;
float compressor_global_height=COMP_TRANSFER_FN_H+COMP_TRANSFER_FN_START_Y+COMP_TRANSFER_FN_END_Y;



RTT_player_UI::RTT_player_UI(rtt_window &_win, string desc, s32 x,s32 y):win(_win){
  player_seek_knob         =win.add_knob2("bitmaps/knobs/player_koorb1.bmp", x+PLAYER_DISPLAY_WIDTH, y    ,0 ,1.0f ,359.0f ,"player" ,"track seek"   ,DONT_RELOAD_FROM_PRESET);win.panes[player_seek_knob        ]->frames_cycle=true;win.panes[player_seek_knob        ]->i_am_a_knob_with_a_momentary_switch_too=true;
  player_track_select_knob =win.add_knob2("bitmaps/knobs/player_koorb2.bmp", x+PLAYER_DISPLAY_WIDTH, y+25 ,0 ,1.0f ,359.0f ,"player" ,"track select" ,DONT_RELOAD_FROM_PRESET);win.panes[player_track_select_knob]->frames_cycle=true;win.panes[player_track_select_knob]->i_am_a_knob_with_a_momentary_switch_too=true;

  player=unique_ptr<RTTplayer>(new RTTplayer());
  rtt_pane &pane=win.add_pane("RTT_player", NULL, NULL, NULL, x ,y, PLAYER_DISPLAY_WIDTH, PLAYER_DISPLAY_HEIGHT);
  pane.pane_special_functionality=PANE_SPECIAL_FUNCTION_PAINT_FUNCTION_IS_GENERIC_STD_FUNCTION_TO_ANIMATE;
  generic_paint_func_for_player_animation=pane.generic_paint_func=std::bind(&RTTplayer::draw, *player, std::ref(pane));
  player_graph_pane_index=win.last_pane_index;
  last_player_seek_knob_displayed_frame        =win.panes[player_seek_knob        ]->displayed_frame;
  last_player_track_select_knob_displayed_frame=win.panes[player_track_select_knob]->displayed_frame;
}



void RTT_player_UI::update_animation(){
  update_controls();
  generic_paint_func_for_player_animation();
  // force redraw onscreen
  rtt_pane &p=*win.panes[player_graph_pane_index];
  win.draw_pane(p,true);
}



void RTT_player_UI::update_controls(){
  rtt_pane &seek_knob_pane         =*(win.panes[player_seek_knob]);
  rtt_pane &track_select_knob_pane =*(win.panes[player_track_select_knob]);
  
  if (seek_knob_pane.momentary_switch_pressed){
    printf("seek_knob_pane.momentary_switch_pressed\n");
    seek_knob_pane.momentary_switch_pressed=false;
    if (player->showing_playlist){
      player->showing_playlist=0;
    }else{
      player->current_genre->jump_to_previous_track();
    }
  }
  if (track_select_knob_pane.momentary_switch_pressed){
    printf("track_select_knob_pane.momentary_switch_pressed\n");
    track_select_knob_pane.momentary_switch_pressed=false;
    if (player->showing_playlist){
      player->current_genre->jump_to_selected_track();
    }else{
      player->current_genre->jump_to_next_track();
    }
  }
  
  if (track_select_knob_pane.displayed_frame!=last_player_track_select_knob_displayed_frame){
    u32 ticks=GetTickCount();
    player->showing_playlist=PLAYER_SHOW_PLAYLIST_FOR_MILLISECONDS+ticks;
    printf("player->showing_playlist = %d\n",player->showing_playlist);
    int delta=track_select_knob_pane.get_modulo_frame_delta(last_player_track_select_knob_displayed_frame);
    printf("delta = %d\n",delta);
    player->current_genre->change_displayed_track_in_playlist(delta);
  }
  if (seek_knob_pane.displayed_frame!=last_player_seek_knob_displayed_frame){
    int delta=seek_knob_pane.get_modulo_frame_delta(last_player_seek_knob_displayed_frame);
    printf("delta = %d\n",delta);
    if (player->showing_playlist){
      printf("player->showing_playlist = %d\n",player->showing_playlist);
      player->showing_playlist=PLAYER_SHOW_PLAYLIST_FOR_MILLISECONDS+GetTickCount();
      player->current_genre->skip_a_prefix_in_playlist(delta);
    }else{
      player->current_genre->seek_playing_track(delta);
    }
  }

  last_player_seek_knob_displayed_frame        =seek_knob_pane        .displayed_frame;
  last_player_track_select_knob_displayed_frame=track_select_knob_pane.displayed_frame;
}




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
  s32 compressor_threshold_frame=win.panes[compressor_threshold_index]->displayed_frame;
  s32 compressor_softness_frame =win.panes[compressor_softness_index ]->displayed_frame;
  s32 compressor_ratio_frame    =win.panes[compressor_ratio_index    ]->displayed_frame;
  s32 compressor_attack_frame   =win.panes[compressor_attack_index   ]->displayed_frame;
  s32 compressor_release_frame  =win.panes[compressor_release_index  ]->displayed_frame;
  s32 limiter_threshold_frame   =win.panes[limiter_threshold_index   ]->displayed_frame;
  s32 limiter_softness_frame    =win.panes[limiter_softness_index    ]->displayed_frame;
  s32 compressor_output_frame   =win.panes[compressor_output_index   ]->displayed_frame;

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
  add_a_compressor(xx,yy+210);
  add_a_player(xx,yy+340);
}


void fxprocessor::add_output_clips_at_knob(s32 xx,s32 yy){
  win.add_knob2("bitmaps/knobs/clips_at_27_2.bmp", 10 ,10, 0, 60.0f, 300.0f, "Output", "output scaling", DONT_RELOAD_FROM_PRESET);
}


void fxprocessor::add_a_compressor(s32 xx,s32 yy){
  compressors.push_back(unique_ptr<compressor>(new compressor(win,"Out compressor",xx,yy)));
  //  compressors.push_back(unique_ptr<compressor>(new compressor(win,"Out compressor",xx,yy+92)));
}


void fxprocessor::add_a_player(s32 xx,s32 yy){
  player=unique_ptr<RTT_player_UI>(new RTT_player_UI(win,"RTT player",xx,yy));
  player_x=xx;
  player_y=yy;
}


void fxprocessor::animation_update(){
  //  printf("tick\n");
  player->update_animation();
}
