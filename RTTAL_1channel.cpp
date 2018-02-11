// scrollher2View.cpp : implementation of the RTTAL class

#include "stdafx.h"
#include <afx.h>
#include <assert.h>
//#include "boost_header.h"
#include "boost/lexical_cast.hpp"
using boost::lexical_cast;

#include <conio.h>
#include <float.h>
#include <fstream>

//#include <math.h>                  // for rttal
//#include <string>                  // for rttal
//#include <vector>                  // for rttal
//#include <deque>                   // for rttal
//#include <map>                     // for rttal
//#include <set>                     // for rttal
//using namespace std;               // for rttal
//#include "imagy.h"                 // for rttal
//#include "RTTcolours.h"            // for rttal
//#include "nkobz.h"                 // for rttal
//#include "IIR2.h"                  // for rttal
//#include "happy_RTA.h"             // for rttal
//#include "RTT_console.h"           // for rttal
//#include "RTT_xwaz.h"              // for rttal
//#include "bass_monitor.h"          // for rttal
//#include "compressher_limither.h"  // for rttal
//#include "rttal.h"                 // for rttal

#include "fxprocess_her_II_headers.h"
//#include "process.h"
//#include "bassher.h"
//#include "equalizer.h"
//#include "widener.h"
//#include "sharpener.h"
//#include "bass_monitor.h"
//#include "compressher_limither.h"
//#include "boost/smart_ptr.hpp"
#include "face meat her.h"
#include "output meat her.h"
#include "crest factor meat her.h"
#include "GBAtext.h"
#include "equalizer.h"
#include "FX_FreakFilturds.h"
#include "schroeder.h"

#include "generic_decoder.h"
#include "wavpack decoder.h"
#include "RTTPlayer.h"
#include "freeverb\revmodel.hpp"
#include "RTTexcursion_monitor.h"
#include "spooler.h"

#include "RTTAL_1channel.h"
//#include "conio.h"

string data_path("");

//void processing::processing(){
//}


int processing::internal_instance_count=0;


void nudge_control(HWND hw,control_pic *c,int dx,int dy){
  c->x+=dx;
  c->y+=dy;
  c->Invalidate_me(hw);
}



void nudge_control_relative(HWND hw,control_pic *c,control_pic *c_counterpart,control_pic *c_base,control_pic *c_counterpart_base){
  c->x=c_base->x+c_counterpart->x-c_counterpart_base->x;
  c->y=c_base->y+c_counterpart->y-c_counterpart_base->y;
  c->Invalidate_me(hw);
}













processing::processing(HWND hw,string _global_channel_name_prefix,int _processor_index,int _plot_x_offset,int _plot_y_offset):global_channel_name_prefix(_global_channel_name_prefix),processor_index(_processor_index),plot_x_offset(_plot_x_offset),plot_y_offset(_plot_y_offset){
  fplayer=new flexi_player(string("bungtron.m3u"));
  fplayer->load_track(1);
  clean_up_timecode_highpass_l=new IIR2("BUTTERWORTH_HIGHPASS_80_140HZ_30dB_CUT_2dB_RIPPLE_TO_CLEAN_UP_SERATO_1Khz_TIMECODE.FDF");
  clean_up_timecode_highpass_r=new IIR2("BUTTERWORTH_HIGHPASS_80_140HZ_30dB_CUT_2dB_RIPPLE_TO_CLEAN_UP_SERATO_1Khz_TIMECODE.FDF");
  internal_index=internal_instance_count++;
  hide_extended_controls=true;
  final_limiter_threshold_knob=0;
  final_limiter_softness_knob=0;
  output_gain_normalized=1.0f;
//  synchronised_flash_frame=0;
  max_output_displayed=0;
  rms_output_smoothed=0;
  max_output_now=0;
  left_buttholmes_down=false;
  need_to_recalc_params_and_use_atomics=true;
  rtaR=0;
  rtaL=0;
  bm=0;
  animate_rta_value=1.0f;
  meat=0;
  final_meat=0;
  crest_factor_meat=0;

  compress_her_bass=0;
  compress_her_ssb=0;
  compress_her_out=0;
  compress_her_final=0;
  volumevolume=0.01f;

  need_redraw_final_compressor=true;
  dbs_of_big_knob_wants_to_be=20.0f;
  dbs_of_big_knob=20.0f;

  bypass_fraction=1;

  ssb_mute_gain_wants_to_be=0;
  output_mute_gain_wants_to_be=0;
  bass_mute_gain_wants_to_be=0;
  final_mute_gain_wants_to_be=0;

  ssb_mute_gain=0;
  output_mute_gain=0;
  bass_mute_gain=0;
  final_mute_gain=0;
  equal_ricer_ellipic9_or_LR4=1;
  schroeder_on_off=0;
  mute_for_reverberator_testing=0;
  equalizer_AH_on_or_off=0.0f;
  equalizer_bass_normalized=FIVEOVERNINE;
  equalizer_mid1_normalized=FIVEOVERNINE;
  equalizer_mid2_normalized=FIVEOVERNINE;
  equalizer_high_normalized=FIVEOVERNINE;
  rms_squared_sum=0;
  rms_num_samples=0;
  schroeder_comb_delay1_normalized=0.1f;
  schroeder_comb_delay2_normalized=0.12f;
  schroeder_comb_delay3_normalized=0.14f;
  schroeder_comb_delay4_normalized=0.2f;
  schroeder_allpass_delay1_normalized=0.3f;
  schroeder_allpass_delay2_normalized=0.4f;
  schroeder_allpass_reverb1_normalized=0.35f;
  schroeder_allpass_reverb2_normalized=0.47f;
  schroeder_reverberation_time_normalized=0.1f;
  schroeder_combs0_7_buffer_length_normalized=0.1f;
  schroeder_mix_normalized=0.1f;
  schroeder_on_off=0;
  schroeder_comb_low_pass_normalized=0.8f;
  schroeder_preset_normalized=0;
  input_gain_normalized=0.5f;
  ssb_compressor_mute=0;
  output_compressor_mute=0;
  bass_compressor_mute=0;
  final_compressor_mute=0;
  last_schnurtgren_guibertube_selector_knob_displayed_frame=-1;
  undo_level=0;
  phase_meater_zoom_rate_normalized=0.75f;
  phase_meater_x_zoom=0.333f;
  phase_meater_update_rate=0.5f;
  phase_meater_master_zoom=0.5f;
  pause_phase_meter_update=false;
  bass_channel_compressor_threshold_normalized=0.5f;
  bass_channel_compressor_softness_normalized=0.9f;
  bass_channel_compressor_ratio_normalized=0.3f;
  bass_channel_limiter_threshold_normalized=0.75f;
  bass_channel_limiter_softness_normalized=0.8f;
  bass_channel_compressor_attack_normalized=0.5f;
  bass_channel_compressor_release_normalized=0.5f;
  bass_channel_dummy_for_compressor_LEDS_normalized=0;
  bass_her_compressor_output_normalized=0.25f;
  output_compressor_output_normalized=0.25f;
  bass_channel_compressor_output_normalized=0.25f;
  bass_her_compressor_threshold_normalized=0.5f;
  output_compressor_threshold_normalized=0.5f;
  output_limiter_threshold_normalized=0.75f;
  final_compressor_threshold_normalized=0.5f;
  final_compressor_threshold_dB=0.5f;
  final_compressor_softness=0.5f;
  final_compressor_attack=10.0f;
  final_compressor_low_release=20.0f;
  final_compressor_ratio=0.5f;
  final_limiter_softness=0.5f;
  final_limiter_threshold_dB=0.5f;
  final_limiter_threshold_normalized=0.75f;
  final_compressor_output_normalized=0.25f;
  graph_offset_in_dBs=0;
  bass_her_compressor_lock=0;
  output_compressor_lock=0;
  bass_channel_compressor_lock=0;
  final_compressor_bypass=0;
  final_compressor_unique=0;
  final_compressor_lock=0;
  bass_her_pass_only_extra_bass_onoff=1.0f;
  last_bass_her_weaver_ssb_onoff=0.0f;
  BASSMON_global_x=300.0f;
  BASSMON_global_y=400.0f;
  BASSMON_global_width =600.0f;
  BASSMON_global_height=86.0f;
  RTAL_global_x=20.0f;
  RTAL_global_y=400.0f;
  RTAL_global_width =600.0f;
  RTAL_global_height=86.0f;
  RTAR_global_x=20.0f;
  RTAR_global_y=400.0f;
  RTAR_global_width =600.0f;
  RTAR_global_height=86.0f;
  normalized_rate_to_track_parameter_changes_nob_val=0.5f;
  bass_her_compressor_softness_normalized=0.9f;
  bass_her_compressor_ratio_normalized=0.3f;
  bass_her_limiter_threshold_normalized=0.75f;
  bass_her_limiter_softness_normalized=0.8f;
  bass_her_compressor_attack_normalized=0.5f;
  bass_her_compressor_release_normalized=0.5f;
  output_level_PUNCHED_UP_TO_THE_MAX_normalized=1.0f;
  output_compressor_softness_normalized=0.9f;
  output_compressor_ratio_normalized=0.3f;
  output_limiter_threshold_normalized=0.75f;
  output_limiter_softness_normalized=0.8f;
  output_compressor_attack_normalized=0.5f;
  output_compressor_release_normalized=0.5f;
  output_dummy_for_compressor_LEDS_normalized=0;
  OUTPUTBASS_global_x=20.0f;
  OUTPUTBASS_global_y=20.0f;
  OUTPUTBASS_global_width =COMP_TRANSFER_FN_W+COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_END_X;
  OUTPUTBASS_global_height=COMP_TRANSFER_FN_H+COMP_TRANSFER_FN_START_Y+COMP_TRANSFER_FN_END_Y;

  bass_channelBASS_global_x=20.0f;
  bass_channelBASS_global_y=20.0f;
  bass_channelBASS_global_width =COMP_TRANSFER_FN_W+COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_END_X;
  bass_channelBASS_global_height=COMP_TRANSFER_FN_H+COMP_TRANSFER_FN_START_Y+COMP_TRANSFER_FN_END_Y;

  final_compressor_softness_normalized=0.9f;
  final_compressor_ratio_normalized=0.3f;
  final_limiter_threshold_normalized=0.75f;
  final_limiter_softness_normalized=0.8f;
  final_compressor_attack_normalized=0.5f;
  final_compressor_release_normalized=0.5f;
  final_dummy_for_compressor_LEDS_normalized=0;

  finalBASS_global_x=20.0f;
  finalBASS_global_y=20.0f;
  finalBASS_global_width =COMP_TRANSFER_FN_W+COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_END_X;
  finalBASS_global_height=COMP_TRANSFER_FN_H+COMP_TRANSFER_FN_START_Y+COMP_TRANSFER_FN_END_Y;

  output_level_instantaneous_normalized=1.0f;
  output_level_headroom_normalized=1.0f;
  output_level_RMS_normalized=1.0f;

  bass_channel_compressor_threshold_dB=0.5f;
  bass_channel_compressor_softness=0.5f;
  bass_channel_compressor_attack=10.0f;
  bass_channel_compressor_low_release=20.0f;
  bass_channel_compressor_ratio=0.5f;
  bass_channel_limiter_softness=0.5f;
  bass_channel_limiter_threshold_dB=0.5f;

  bass_her_compressor_threshold_dB=0.5f;
  bass_her_compressor_softness=0.5f;
  bass_her_compressor_attack=10.0f;
  bass_her_compressor_low_release=20.0f;
  bass_her_compressor_ratio=0.5f;
  bass_her_limiter_softness=0.5f;
  bass_her_limiter_threshold_dB=0.5f;

  output_compressor_threshold_dB=0.5f;
  output_compressor_softness=0.5f;
  output_compressor_attack=10.0f;
  output_compressor_low_release=20.0f;
  output_compressor_ratio=0.5f;
  output_limiter_softness=0.5f;
  output_limiter_threshold_dB=0.5f;


  RTAL_global_x=20.0f;
  RTAL_global_y=400.0f;
  RTAL_global_width =474.0f;
  RTAL_global_height=86.0f;
  
  RTAR_global_x=20.0f;
  RTAR_global_y=400.0f;
  RTAR_global_width =474.0f;
  RTAR_global_height=86.0f;

  COMPSSB_global_x=20.0f;
  COMPSSB_global_y=20.0f;
  COMPSSB_global_width =COMP_TRANSFER_FN_W+COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_END_X;
  COMPSSB_global_height=COMP_TRANSFER_FN_H+COMP_TRANSFER_FN_START_Y+COMP_TRANSFER_FN_END_Y;

  bass_her_dummy_for_compressor_LEDS_normalized=0;

  bass_her_compressor_bypass=0;
  bass_her_compressor_unique=0;
  output_compressor_bypass=0;
  output_compressor_unique=0;
  bass_channel_compressor_bypass=0;
  bass_channel_compressor_unique=0;

  global_sharpen_ammount=0.5f;

  global_bas_gain_wants_to_be=0.5f;
  global_mid_gain_wants_to_be=0.5f;
  global_top_gain_wants_to_be=0.5f;
  global_bas_width=0.0f;
  global_mid_width=1.0f;
  global_top_width=1.0f;

  rta_shows_peaks=0.0f;

  bass_her_weaver_ssb_onoff=0.0f;

  widen_the_nig=1.0f;
  sharpen_the_nig=1.0f;
  equalize_the_nig=1.0f;

  bass_her_ammount_of_bass_boost_wants_to_be=0.5f;
  bass_her_pass_only_extra_bass_onoff=1.0f;
  bass_her_clean_up_highpass_index_normalized=.3f;

  weaver_ssb_centre_low_pass_filter_index_normalized=0.5f;

  bass_her_clean_up_lowpass_index_blending_from=-1;
  bass_her_clean_up_highpass_index_blending_from=-1;

  bass_her_clean_up_low_pass_index_normalized=.5f;

  need_redraw_final_compressor=true;

  weighting_curve_float=1.0f;

  auto_load_presets_on_off_radio=1.0f;

  save_preset_radio=0;



  create_knobs();
  meat=new phase_meter((int)dummy_for_face_meat_her_knob->x,(int)dummy_for_face_meat_her_knob->y,(int)dummy_for_face_meat_her_knob->width,(int)dummy_for_face_meat_her_knob->height);
  final_meat=new output_meter((int)dummy_for_output_meter_knob->x,(int)dummy_for_output_meter_knob->y,(int)dummy_for_output_meter_knob->width,(int)dummy_for_output_meter_knob->height);
  crest_factor_meat=new crest_factor_meter((int)dummy_for_crest_factor_meter_knob->x,(int)dummy_for_crest_factor_meter_knob->y,(int)dummy_for_crest_factor_meter_knob->width,(int)dummy_for_crest_factor_meter_knob->height);

  my_noco=new rtt_console((float)dummy_for_console_knob->x,(float)dummy_for_console_knob->y,INDIVIDUAL_DECK_CONSOLE_WIDTH,INDIVIDUAL_DECK_CONSOLE_HEIGHT,grey65_cr,coral2_cr,green3_cr);
  my_noco->m(hw,"Audio Processor initialized"); 

  xwaz_console=new rtt_xwaz_console((float)dummy_for_xwaz_display_knob->x,(float)dummy_for_xwaz_display_knob->y,INDIVIDUAL_DECK_XWAZ_CONSOLE_WIDTH,INDIVIDUAL_DECK_XWAZ_CONSOLE_HEIGHT,grey65_cr,coral2_cr,green3_cr);
  xwaz_console->m(hw,"xwaz console initialized"); 

  setup_processing();
  check_UI_movement_things(hw);
  load_knobs_to_hide_for_minimal_ui();
  xw.co=xwaz_console;
}







void processing::setup_equalizers(){
  equalizer_ah_l[0].set_freq(EQUALIZER_ALLEN_AND_HEAVE_LO_SHELF);
  equalizer_ah_r[0].set_freq(EQUALIZER_ALLEN_AND_HEAVE_LO_SHELF);
  equalizer_ah_l[0].set_q(EQUALIZER_ALLEN_AND_HEAVE_LO_SHELF_Q);
  equalizer_ah_r[0].set_q(EQUALIZER_ALLEN_AND_HEAVE_LO_SHELF_Q);
  equalizer_ah_l[0].set_type(FILTER_TYPE_LOW_SHELF);
  equalizer_ah_r[0].set_type(FILTER_TYPE_LOW_SHELF);
  float bass_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_bass_normalized);
  equalizer_ah_l[0].set_dbgain(bass_gain);
  equalizer_ah_r[0].set_dbgain(bass_gain);

  equalizer_ah_l[1].set_freq(EQUALIZER_ALLEN_AND_HEAVE_MID1);
  equalizer_ah_r[1].set_freq(EQUALIZER_ALLEN_AND_HEAVE_MID1);
  equalizer_ah_l[1].set_q(EQUALIZER_ALLEN_AND_HEAVE_MID1_Q);
  equalizer_ah_r[1].set_q(EQUALIZER_ALLEN_AND_HEAVE_MID1_Q);
  equalizer_ah_l[1].set_type(FILTER_TYPE_PEAKING);
  equalizer_ah_r[1].set_type(FILTER_TYPE_PEAKING);
  float mid1_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_mid1_normalized);
  equalizer_ah_l[1].set_dbgain(mid1_gain);
  equalizer_ah_r[1].set_dbgain(mid1_gain);

  equalizer_ah_l[2].set_freq(EQUALIZER_ALLEN_AND_HEAVE_MID2);
  equalizer_ah_r[2].set_freq(EQUALIZER_ALLEN_AND_HEAVE_MID2);
  equalizer_ah_l[2].set_q(EQUALIZER_ALLEN_AND_HEAVE_LO_SHELF_Q);
  equalizer_ah_r[2].set_q(EQUALIZER_ALLEN_AND_HEAVE_LO_SHELF_Q);
  equalizer_ah_l[2].set_type(FILTER_TYPE_PEAKING);
  equalizer_ah_r[2].set_type(FILTER_TYPE_PEAKING);
  float mid2_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_mid2_normalized);
  equalizer_ah_l[2].set_dbgain(mid2_gain);
  equalizer_ah_r[2].set_dbgain(mid2_gain);

  equalizer_ah_l[3].set_freq(EQUALIZER_ALLEN_AND_HEAVE_HI_SHELF);
  equalizer_ah_r[3].set_freq(EQUALIZER_ALLEN_AND_HEAVE_HI_SHELF);
  equalizer_ah_l[3].set_q(EQUALIZER_ALLEN_AND_HEAVE_HI_SHELF_Q);
  equalizer_ah_r[3].set_q(EQUALIZER_ALLEN_AND_HEAVE_HI_SHELF_Q);
  equalizer_ah_l[3].set_type(FILTER_TYPE_HIGH_SHELF);
  equalizer_ah_r[3].set_type(FILTER_TYPE_HIGH_SHELF);
  float high_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_high_normalized);
  equalizer_ah_l[3].set_dbgain(high_gain);
  equalizer_ah_r[3].set_dbgain(high_gain);

  //  float a=get_ah_mid_gain_from_normalized_dbs(0);
  //  float b=get_ah_mid_gain_from_normalized_dbs(0.25);
  //  float c=get_ah_mid_gain_from_normalized_dbs(0.5);
  //  float d=get_ah_mid_gain_from_normalized_dbs(FIVEOVERNINE);
  //  float e=get_ah_mid_gain_from_normalized_dbs(1.0);
}






void processing::setup_processing(){
  //#define  ENABLE_FLOATING_POINT_EXCEPTIONS {\
  //  unsigned int control_word;\
  //  _controlfp_s(&control_word,  _EM_UNDERFLOW|_EM_INEXACT|_EM_OVERFLOW|_EM_ZERODIVIDE|_EM_INVALID|_EM_DENORMAL , _MCW_EM);}
  //#define  ENABLE_FLOATING_POINT_EXCEPTIONS_EXCEPT_INEXACT _control87(_EM_INVALID|_EM_INEXACT, _MCW_EM);

#ifdef _DEBUG
  ENABLE_FLOATING_POINT_EXCEPTIONS_EXCEPT_INEXACT 
#endif
    setup_equalizers();
  player=new RTTplayer();
  excurse=new RTTexcurse();

  rtaL=new happy_rta(RTAL_global_x,RTAL_global_y,RTAL_global_width,RTAL_global_height);
  rtaR=new happy_rta(RTAR_global_x,RTAR_global_y,RTAR_global_width,RTAR_global_height);
  bm=new bass_monitor(BASSMON_global_x,BASSMON_global_y,BASSMON_global_width,BASSMON_global_height);
 compress_her_ssb=new compress_her_limit_her(SUB_BASS_COMPRESSOR_DESC_STRING,COMPSSB_global_x,COMPSSB_global_y,COMPSSB_global_width,COMPSSB_global_height,compress_her_ssb_LED_rect_global);
  compress_her_bass=new compress_her_limit_her(BASS_COMPRESSOR_DESC_STRING,bass_channelBASS_global_x+200,bass_channelBASS_global_y,bass_channelBASS_global_width,bass_channelBASS_global_height,compress_her_bass_LED_rect_global);
  compress_her_out=new compress_her_limit_her(MID_HIGH_COMPRESSOR_DESC_STRING,OUTPUTBASS_global_x-200,OUTPUTBASS_global_y,OUTPUTBASS_global_width,OUTPUTBASS_global_height,compress_her_out_LED_rect_global);
  compress_her_final=new compress_her_limit_her(FINAL_MIX_COMPRESSOR_DESC_STRING,finalBASS_global_x-200,finalBASS_global_y,finalBASS_global_width,finalBASS_global_height,compress_her_out_LED_rect_global);
}











void processing::smooth_me_quick(float &value_use,float value_wants_to_be){
  if (value_use<value_wants_to_be){
    value_use+=SMOOTH_ME_QUICK_VALUE;
    if (fabsf(value_use-value_wants_to_be)<=SMOOTH_ME_QUICK_VALUE){
      value_use=value_wants_to_be;
    }
  }else if(value_use>value_wants_to_be){
    value_use-=SMOOTH_ME_QUICK_VALUE;
    if (fabsf(value_use-value_wants_to_be)<=SMOOTH_ME_QUICK_VALUE){
      value_use=value_wants_to_be;
    }
  }
}

#define SMOOTH_ME_OUTPUT_GAIN_CHANGES_VALUE (1.0f/(0.2f*44100.0f))

void processing::smooth_me_output_gain_changes(float &value_use,float value_wants_to_be){
  if (value_use<value_wants_to_be){
    value_use+=SMOOTH_ME_OUTPUT_GAIN_CHANGES_VALUE ;
    if (fabsf(value_use-value_wants_to_be)<=SMOOTH_ME_OUTPUT_GAIN_CHANGES_VALUE){
      value_use=value_wants_to_be;
    }
  }else if(value_use>value_wants_to_be){
    value_use-=SMOOTH_ME_OUTPUT_GAIN_CHANGES_VALUE ;
    if (fabsf(value_use-value_wants_to_be)<=SMOOTH_ME_OUTPUT_GAIN_CHANGES_VALUE){
      value_use=value_wants_to_be;
    }
  }
}

void processing::atomic_update_of_all_stuff_that_might_cause_clipping(){
  smooth_me_output_gain_changes(dbs_of_big_knob,dbs_of_big_knob_wants_to_be);
  //  dbs_of_big_knob=dbs_of_big_knob_wants_to_be;
  compress_her_final->limiter->limit=dbs_of_big_knob;
  volumevolume=pow(10.0,-(double)dbs_of_big_knob*0.1);
  compress_her_final->limiter->recalc_params();
  compress_her_final->take_limiter_snapshot();
  if (dbs_of_big_knob==dbs_of_big_knob_wants_to_be){
    need_to_recalc_params_and_use_atomics=false;
  }else{
    need_redraw_final_compressor=true;
    compress_her_final->needs_redraw=true;
  }
}



float processing::get_ah_mid_gain_from_normalized_dbs(float v){
  if (v>FIVEOVERNINE){
    float param=(v-FIVEOVERNINE)/(1.0f-FIVEOVERNINE);
    return param*EQUALIZER_ALLEN_AND_HEAVE_MAX_BOOST;
  }
  float param=(FIVEOVERNINE-v)/FIVEOVERNINE;
  return -param*EQUALIZER_ALLEN_AND_HEAVE_MAX_CUT;
}


extern float lt[10],rt[10];


void processing::doit( float &l, float &r ){
  xw.add_sample(l,r);
//  l=clean_up_timecode_highpass_l->doit(l);
//  r=clean_up_timecode_highpass_r->doit(r);

  if (need_to_recalc_params_and_use_atomics){
    atomic_update_of_all_stuff_that_might_cause_clipping();
  }
  if (RTTSG_on_or_off==0){
    float old_l=l;
    float old_r=r;
    smooth_me_quick(bypass_fraction,1.0f-RTTSG_bypass_on_or_off);
    smooth_me_quick(ssb_mute_gain,   ssb_mute_gain_wants_to_be);;
    smooth_me_quick(output_mute_gain,output_mute_gain_wants_to_be);
    smooth_me_quick(bass_mute_gain,  bass_mute_gain_wants_to_be);
    smooth_me_quick(final_mute_gain, final_mute_gain_wants_to_be);
    if (bypass_fraction!=1.0f){
      //      assert(l<1.1f && l>-1.1f);
      //      assert(r<1.1f && r>-1.1f);
      float original_signal_bass_l;
      float original_signal_bass_r;
      float ol=l;
      float or=r;
      if (equal_ricer_ellipic9_or_LR4){
        widener.doitLR4(l,r,original_signal_bass_l,original_signal_bass_r);
      }else{
        widener.doitIIR2(l,r,original_signal_bass_l,original_signal_bass_r);
      }
      lt[0]=l;
      rt[0]=r;
      l=sharpenL.doit(l);
      r=sharpenR.doit(r);
      lt[1]=l;
      rt[1]=r;
      float ssb_bass_l=original_signal_bass_l;
      float ssb_bass_r=original_signal_bass_r;
//      lt[1]=ssb_bass_l;
//      rt[1]=ssb_bass_r;
      float attenation_from_SSB_unique=bassher.doit(ssb_bass_l,ssb_bass_r,compress_her_ssb,this);
      ssb_bass_l*=ssb_mute_gain;
      ssb_bass_r*=ssb_mute_gain;

      float summed_bass_l=original_signal_bass_l*attenation_from_SSB_unique+ssb_bass_l;
      float summed_bass_r=original_signal_bass_r*attenation_from_SSB_unique+ssb_bass_r;
      compress_her_bass->doit(summed_bass_l,summed_bass_r,false);
      summed_bass_l*=bass_mute_gain;
      summed_bass_r*=bass_mute_gain;
      lt[2]=summed_bass_l;
      rt[2]=summed_bass_r;

      if (schroeder_on_off==0){
        if (mute_for_reverberator_testing!=0){
          l=0;
          r=0;
          summed_bass_l=0;
          summed_bass_r=0;
        }
        global_reverb.Process(l,r);
      }
      lt[3]=l;
      rt[3]=r;
      compress_her_final->doit_comrpessor_only(l,r); // fast compressor first
      compress_her_out->doit(l,r,false);
      l*=output_mute_gain;
      r*=output_mute_gain;
      lt[4]=l;
      rt[4]=r;
      float compressed_midtop_l=l;
      float compressed_midtop_r=r;
      l=l*attenation_from_SSB_unique+summed_bass_l;
      r=r*attenation_from_SSB_unique+summed_bass_r;
      lt[4]=l;
      rt[4]=r;

      bool final_limiter_only=true;
      if (bass_her_compressor_unique_knob_global->displayed_frame!=0){
        l=ssb_bass_l;
        r=ssb_bass_r;
      }else if (bass_channel_compressor_unique_knob_global->displayed_frame!=0){
        l=summed_bass_l;
        r=summed_bass_r;
      }else if (output_compressor_unique_knob_global->displayed_frame!=0){
        l=compressed_midtop_l;
        r=compressed_midtop_r;
      }else{
        final_limiter_only=false;
      }
      lt[5]=l;
      rt[5]=r;
#ifdef _DEBUG
      test_for_clipping=true;
#endif
      max_output=max(max_output,fabsf(l));
      max_output=max(max_output,fabsf(r));

      compress_her_final->doit_limiter_only(l,r,true);

      l*=final_mute_gain;
      r*=final_mute_gain;
      lt[6]=l;
      rt[6]=r;

#ifdef _DEBUG
      test_for_clipping=false;
#endif

      if (equalizer_AH_on_or_off==0.0){
        l=equalizer_ah_l[0].process_biquad(l);
        r=equalizer_ah_r[0].process_biquad(r);
        l=equalizer_ah_l[1].process_biquad(l);
        r=equalizer_ah_r[1].process_biquad(r);
        l=equalizer_ah_l[2].process_biquad(l);
        r=equalizer_ah_r[2].process_biquad(r);
        l=equalizer_ah_l[3].process_biquad(l);
        r=equalizer_ah_r[3].process_biquad(r);
      }
      float bass_gain=compress_her_bass->get_current_gain();
      float final_gain=compress_her_final->get_current_gain();
      float overall_bass_gain=bass_gain*final_gain;
      float sblf=summed_bass_l*final_gain;
      float sbrf=summed_bass_r*final_gain;
      meat->doit(l,r,sblf,sbrf);
      final_meat->doit(l,r,ssb_bass_l*overall_bass_gain,ssb_bass_r*overall_bass_gain);
      crest_factor_meat->doit(l,r,sblf,sbrf);
    }
    lt[7]=l;
    rt[7]=r;
    float ifrac=1.0f-bypass_fraction;
    l=old_l*bypass_fraction+l*ifrac;
    r=old_r*bypass_fraction+r*ifrac;
    animate_rta_value=1.0f;
    rtaL->feed_me(l*0.5f,l*0.5f);
    rtaR->feed_me(r*0.5f,r*0.5f);
    lt[8]=l;
    rt[8]=r;
  }else{
    //    if (only_reverb){
    //      if (schroeder_on_off==0){
    //        global_reverb.Process(l,r);
    //      }
    //      rtaL->feed_me(l,l);
    //      rtaR->feed_me(r,r);
    //    }else{
    if (animate_rta_value>1e-10f){
      float v=(0.000001f)*(float)rand()/(float)RAND_MAX;
      rtaL->feed_me(l*animate_rta_value+v,l*animate_rta_value+v);
      rtaR->feed_me(r*animate_rta_value+v,r*animate_rta_value+v);
      animate_rta_value*=0.9998f;
    }else{
      rtaL->pause=true;
      rtaR->pause=true;
    }
    //    }
    max_output=max(max_output,max(l,r));
  }
  rms_squared_sum+=max_output*max_output;
  rms_num_samples++;
  l*=0.1f;
  r*=0.1f;
}















void processing::create_undo_level(){
  for(int i=0;i<(int)knobs.size();i++){
    knobs[i]->set_undo_level(undo_level);
  }
  undo_level++;
}



void processing::dump_preset2(HWND hw,int n){
  string fn=data_path+"_preset_"+lexical_cast<string>(n)+"_2.txt";
  ofstream of(fn.data(),ios::app);
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *c=knobs[i];
    of<<c->group<<"____"<<c->desc<<endl;
    of<<c->displayed_frame<<endl;
  }
  of.close();
  noco->m(hw,("overwrote preset "+lexical_cast<string>(n)).data());
}



void processing::load_preset2(HWND hw,int n){
  create_undo_level();
  string fn=data_path+"_preset_"+lexical_cast<string>(n)+"_2.txt";
  ifstream of(fn.data());
  if (of){
    map<string,int> presets;
    string data;
    while(!of.eof()){
      string key;
      if (getline(of,key)){
        if (getline(of,data)){
          presets[key]=lexical_cast<int>(data);
        }
      }
    }
    of.close();
    for(int i=0;i<(int)knobs.size();i++){
      control_pic *k=knobs[i];
      string hash=k->hash_name();
      if (presets.find(hash)!=presets.end()){
        if (!k->dont_reload_me){
          k->set_normalized_float_value_from_frame(presets[hash]);
          k->Invalidate_me(hw);
        }
      }else{
//        assert(0);
      }
    }
  }
  noco->m(hw,("loaded preset "+lexical_cast<string>(n)).data());
//  dump_preset2(hw,n);

}





void processing::save_knobs_to_hide_for_minimal_ui(){
  string fn=data_path+"_controls_to_hide_to_toggle_minimal_UI_"+lexical_cast<string>(processor_index)+"_.txt";
  ofstream f(fn.data());
  if (f){
    set<string>::iterator itr;
    for(itr=controls_to_hide_to_toggle_minimal_UI.begin();itr!=controls_to_hide_to_toggle_minimal_UI.end();itr++){
      f<<*itr<<endl;
    }
    f.close();
  }
}




void processing::load_knobs_to_hide_for_minimal_ui(){
  string fn=data_path+"_controls_to_hide_to_toggle_minimal_UI_"+lexical_cast<string>(processor_index)+"_.txt";
  ifstream f(fn.data());
  controls_to_hide_to_toggle_minimal_UI.clear();
  if (f){
    while(!f.eof()){
      string data;
      getline(f,data);
      controls_to_hide_to_toggle_minimal_UI.insert(data);
    }
    f.close();
  }
  recalc_hidden_controls();
}





bool processing::check_knobs_for_movement_if_selected_and_mouse_moved_while_left_button_is_held_down(HWND hwnd,POINT *point) {
  bool any_moved=false;
  for(int i=0;i<(int)knobs.size();i++){
    if (knobs[i]->set_position_or_drag_control(hwnd,point)){
      any_moved=true;
    }
  }
  return any_moved;
}








void processing::do_a_global_reeeeeeefreshhhhh(HWND hw){
  rttal->do_a_global_reeeeeeefreshhhhh(hw);
//  SetTimer(hw,TIMER_EVENT_DO_A_BIG_REDRAW,1000,0);
}






void processing::OnMouseMove(HWND hw,UINT nFlags, POINT point){
  called_from_inside_windows_paint_function_or_must_redraw=false;
  bool check_special_UI_movement=false;
  bool show_help_for_lusers=true;
  if (left_buttholmes_down){
    if (GetAsyncKeyState(VK_CONTROL)&0x8000){
      called_from_inside_windows_paint_function_or_must_redraw=true;
    }
    if (check_knobs_for_movement_if_selected_and_mouse_moved_while_left_button_is_held_down(hw,&point)){
      check_special_UI_movement=true;
      if (GetAsyncKeyState(VK_CONTROL)&0x8000){
        redraw_unredraw_with_invalidate_things(hw);
        do_a_global_reeeeeeefreshhhhh(hw);
      }
    }
    show_help_for_lusers=false;
  }
  if (check_special_UI_movement){
    check_UI_movement_things(hw);
    show_help_for_lusers=false;
  }
  if (show_help_for_lusers){
    static int which_control_was_mouse_over_last_time=-1;
    for(int i=0;i<(int)knobs.size();i++){
      control_pic *k=knobs[i];
      bool is_hidden=k->hidden&hide_extended_controls;
      if (!is_hidden && k->is_pt_inside_control_rectangular_region_influence_not_the_control_specific_shape_are_we_clear_on_that(&point)){
        if (which_control_was_mouse_over_last_time!=i){
          noco->m(hw,"dis is: "+k->desc+"  control");
        }
        which_control_was_mouse_over_last_time=i;
        break;
      }
    }
  }
}








void processing::do_rta_radio(HWND hw){
  DWORD tr1=rta_mode_select_normal_radio->time_user_last_clicked_or_operated;
  DWORD tr2=rta_mode_select_text_radio->time_user_last_clicked_or_operated;
  DWORD tr3=rta_mode_select_text_above_radio->time_user_last_clicked_or_operated;
  int r=1;
  if (tr2>tr1){
    r=2;
  }
  if (tr3>tr2 && tr3>tr1){
    r=3;
  }
  bool redraw_radio_buttons=false;
  switch(r){
  case 1:
    if (rta_mode_select_normal_radio->displayed_frame!=1 || rta_mode_select_text_radio->displayed_frame!=0 || rta_mode_select_text_above_radio->displayed_frame!=0){
      noco->m(hw,"RTA set to display instantaneous value mode");
      rta_mode_select_normal_radio->set_normalized_float_value_from_frame(1);
      rta_mode_select_text_radio->set_control_to_this_normalized_float_value(0);
      rta_mode_select_text_above_radio->set_control_to_this_normalized_float_value(0);
      redraw_radio_buttons=true;
      happy_rta_style=0;
    }
    break;
  case 2:
    if (rta_mode_select_normal_radio->displayed_frame!=0 || rta_mode_select_text_radio->displayed_frame!=1 || rta_mode_select_text_above_radio->displayed_frame!=0){
      noco->m(hw,"RTA set to display instantaneous value text mode");
      rta_mode_select_normal_radio->set_normalized_float_value_from_frame(0);
      rta_mode_select_text_radio->set_control_to_this_normalized_float_value(1);
      rta_mode_select_text_above_radio->set_control_to_this_normalized_float_value(0);
      redraw_radio_buttons=true;
      happy_rta_style=1;
    }
    break;
  case 3:
    if (rta_mode_select_normal_radio->displayed_frame!=0 || rta_mode_select_text_radio->displayed_frame!=0 || rta_mode_select_text_above_radio->displayed_frame!=1){
      noco->m(hw,"RTA set to display instantaneous value text above mode");
      rta_mode_select_normal_radio->set_normalized_float_value_from_frame(0);
      rta_mode_select_text_radio->set_control_to_this_normalized_float_value(0);
      rta_mode_select_text_above_radio->set_control_to_this_normalized_float_value(1);
      redraw_radio_buttons=true;
      happy_rta_style=2;
    }
    break;
  }
  if (redraw_radio_buttons){
    rta_mode_select_normal_radio->Invalidate_me(hw);
    rta_mode_select_text_radio->Invalidate_me(hw);
    rta_mode_select_text_above_radio->Invalidate_me(hw);
  }
}















void processing::undo(HWND hwnd){
  if (knobs[0]->num_undo_levels()>0){
    undo_level--;
    if (undo_level<0){
      undo_level=0;
    }
    for(int i=0;i<(int)knobs.size();i++){
      knobs[i]->undo(undo_level);
    }
  }
  check_UI_movement_things(hwnd);
  InvalidateRect(hwnd,0,false);
}



void processing::redo(HWND hwnd){
  undo_level++;
  if (undo_level>=knobs[0]->num_undo_levels()){
    undo_level--;
  }
  for(int i=0;i<(int)knobs.size();i++){
    knobs[i]->undo(undo_level);
  }
  check_UI_movement_things(hwnd);
  InvalidateRect(hwnd,0,false);
}



void processing::adjust_final_output_limiter_threshold_for_output_gain_control(HWND hw){
  dbs_of_big_knob_wants_to_be=output_gain_normalized*20.0f;
  need_to_recalc_params_and_use_atomics=true;
  final_limiter_threshold_knob->set_control_to_this_normalized_float_value(0.5f+dbs_of_big_knob_wants_to_be*0.025f);
  final_limiter_threshold_knob->Invalidate_me(hw);
}



















void processing::check_UI_movement_things(HWND hw,bool just_loaded_a_new_preset){

  static float last_schroeder_comb_delay1=-1;
  static float last_schroeder_comb_delay2=-1;
  static float last_schroeder_comb_delay3=-1;
  static float last_schroeder_comb_delay4=-1;
  static float last_schroeder_allpass_delay1=-1;
  static float last_schroeder_allpass_delay2=-1;
  static float last_schroeder_allpass_reverb1=-1;
  static float last_schroeder_allpass_reverb2=-1;
  static float last_schroeder_reverberation_time=-1;
  static float last_combs_5_12_buffer_length_modifier_override=-1;
  static float last_schroeder_mix=-1;
  static float last_schroeder_on_off=-1;
  static float last_schroeder_comb_low_pass_normalized=-1;
  static float last_schroeder_preset_normalized=-1;
  bool recalc_reveb=false;
  if (last_schroeder_comb_delay1!=schroeder_comb_delay1_normalized){
    global_reverb.set_comb_delay(0,schroeder_comb_delay1_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_comb_delay2!=schroeder_comb_delay2_normalized){
    global_reverb.set_comb_delay(1,schroeder_comb_delay2_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_comb_delay3!=schroeder_comb_delay3_normalized){
    global_reverb.set_comb_delay(2,schroeder_comb_delay3_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_comb_delay4!=schroeder_comb_delay4_normalized){
    global_reverb.set_comb_delay(3,schroeder_comb_delay4_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_allpass_delay1!=schroeder_allpass_delay1_normalized){
    global_reverb.set_allpass_delay(0,schroeder_allpass_delay1_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_allpass_delay2!=schroeder_allpass_delay2_normalized){
    global_reverb.set_allpass_delay(1,schroeder_allpass_delay2_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_allpass_reverb1!=schroeder_allpass_reverb1_normalized){
    global_reverb.set_allpass_reverb(0,schroeder_allpass_reverb1_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_allpass_reverb2!=schroeder_allpass_reverb2_normalized){
    global_reverb.set_allpass_reverb(1,schroeder_allpass_reverb2_normalized);
    recalc_reveb=true;
  }
  if (last_schroeder_reverberation_time!=schroeder_reverberation_time_normalized){
    global_reverb.set_reverberation_time(schroeder_reverberation_time_normalized);
    recalc_reveb=true;
  }
  if (last_combs_5_12_buffer_length_modifier_override!=schroeder_combs0_7_buffer_length_normalized){
    global_reverb.set_combs_5_12_buffer_length_modifier_override(schroeder_combs0_7_buffer_length_normalized);
    recalc_reveb=true;
  }
  //  static float last_combs_5_12_buffer_length_modifier_override=-1;

  if (last_schroeder_mix!=schroeder_mix_normalized){
    global_reverb.wet_dry=schroeder_mix_normalized;
    recalc_reveb=true;
  }
  if (last_schroeder_preset_normalized!=schroeder_preset_normalized){
    global_reverb.reverb_assign_preset(schroeder_preset_knob->displayed_frame);
    set_schroeder_reverb_UI_knobz();
    InvalidateRect(hw,0,false);
  }
  if (last_schroeder_on_off!=schroeder_on_off){
  }
  if (last_schroeder_comb_low_pass_normalized!=schroeder_comb_low_pass_normalized){
    global_reverb.set_comb_filter_low_pass_frequency(schroeder_comb_low_pass_normalized);
    recalc_reveb=true;
  }
  if (recalc_reveb){
    global_reverb.calculate_reverb_parameters();
  }
  last_schroeder_comb_delay1=schroeder_comb_delay1_normalized;
  last_schroeder_comb_delay2=schroeder_comb_delay2_normalized;
  last_schroeder_comb_delay3=schroeder_comb_delay3_normalized;
  last_schroeder_comb_delay4=schroeder_comb_delay4_normalized;
  last_schroeder_allpass_delay1=schroeder_allpass_delay1_normalized;
  last_schroeder_allpass_delay2=schroeder_allpass_delay2_normalized;
  last_schroeder_allpass_reverb1=schroeder_allpass_reverb1_normalized;
  last_schroeder_allpass_reverb2=schroeder_allpass_reverb2_normalized;
  last_schroeder_reverberation_time=schroeder_reverberation_time_normalized;
  last_combs_5_12_buffer_length_modifier_override=schroeder_combs0_7_buffer_length_normalized;
  last_schroeder_mix=schroeder_mix_normalized;
  last_schroeder_preset_normalized=schroeder_preset_normalized;
  last_schroeder_on_off=schroeder_on_off;
  last_schroeder_comb_low_pass_normalized=schroeder_comb_low_pass_normalized;

  input_gain=powf(10.0f,(input_gain_normalized-0.5f)*2.0f);

  static float last_equalizer_bass_normalized=-1;
  static float last_equalizer_mid1_normalized=-1;
  static float last_equalizer_mid2_normalized=-1;
  static float last_equalizer_high_normalized=-1;
  if (last_equalizer_bass_normalized!=equalizer_bass_normalized){
    float bass_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_bass_normalized);
    equalizer_ah_l[0].set_dbgain(bass_gain);
    equalizer_ah_r[0].set_dbgain(bass_gain);
  }
  if (last_equalizer_mid1_normalized!=equalizer_mid1_normalized){
    float mid1_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_mid1_normalized);
    equalizer_ah_l[1].set_dbgain(mid1_gain);
    equalizer_ah_r[1].set_dbgain(mid1_gain);
  }
  if (last_equalizer_mid2_normalized!=equalizer_mid2_normalized){
    float mid2_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_mid2_normalized);
    equalizer_ah_l[2].set_dbgain(mid2_gain);
    equalizer_ah_r[2].set_dbgain(mid2_gain);
  }
  if (last_equalizer_high_normalized!=equalizer_high_normalized){
    float high_gain=get_ah_mid_gain_from_normalized_dbs(equalizer_high_normalized);
    equalizer_ah_l[3].set_dbgain(high_gain);
    equalizer_ah_r[3].set_dbgain(high_gain);
  }
  last_equalizer_bass_normalized=equalizer_bass_normalized;
  last_equalizer_mid1_normalized=equalizer_mid1_normalized;
  last_equalizer_mid2_normalized=equalizer_mid2_normalized;
  last_equalizer_high_normalized=equalizer_high_normalized;

  ssb_mute_gain_wants_to_be=1.0f-ssb_compressor_mute;
  output_mute_gain_wants_to_be=1.0f-output_compressor_mute;
  bass_mute_gain_wants_to_be=1.0f-bass_compressor_mute;
  final_mute_gain_wants_to_be=1.0f-final_compressor_mute;

  if (last_schnurtgren_guibertube_selector_knob_displayed_frame!=schnurtgren_guibertube_selector_knob->displayed_frame){
    last_schnurtgren_guibertube_selector_knob_displayed_frame=schnurtgren_guibertube_selector_knob->displayed_frame;
    if (auto_load_presets_on_off_radio){
      load_preset2(hw,schnurtgren_guibertube_selector_knob->displayed_frame);
    }
  }
  static int last_output_level_PUNCHED_UP_TO_THE_MAX_knob_x=output_level_PUNCHED_UP_TO_THE_MAX_knob->x;
  static int last_output_level_instantaneous_knob_x=output_level_instantaneous_knob->x;
  static int last_output_level_RMS_knob_x=output_level_RMS_knob->x;
  static int last_output_level_headroom_knob_x=output_level_headroom_knob->x;
  if (last_output_level_PUNCHED_UP_TO_THE_MAX_knob_x!=output_level_PUNCHED_UP_TO_THE_MAX_knob->x){
    output_level_PUNCHED_UP_TO_THE_MAX_knob->x=output_level_PUNCHED_UP_TO_THE_MAX_knob->x;
    output_level_instantaneous_knob->x=output_level_PUNCHED_UP_TO_THE_MAX_knob->x;
    output_level_RMS_knob->x=output_level_PUNCHED_UP_TO_THE_MAX_knob->x;
    output_level_headroom_knob->x=output_level_PUNCHED_UP_TO_THE_MAX_knob->x;
  }
  if (last_output_level_instantaneous_knob_x!=output_level_instantaneous_knob->x){
    output_level_PUNCHED_UP_TO_THE_MAX_knob->x=output_level_instantaneous_knob->x;
    output_level_instantaneous_knob->x=output_level_instantaneous_knob->x;
    output_level_RMS_knob->x=output_level_instantaneous_knob->x;
    output_level_headroom_knob->x=output_level_instantaneous_knob->x;
  }
  if (last_output_level_RMS_knob_x!=output_level_RMS_knob->x){
    output_level_PUNCHED_UP_TO_THE_MAX_knob->x=output_level_RMS_knob->x;
    output_level_instantaneous_knob->x=output_level_RMS_knob->x;
    output_level_RMS_knob->x=output_level_RMS_knob->x;
    output_level_headroom_knob->x=output_level_RMS_knob->x;
  }
  if (last_output_level_headroom_knob_x!=output_level_headroom_knob->x){
    output_level_PUNCHED_UP_TO_THE_MAX_knob->x=output_level_headroom_knob->x;
    output_level_instantaneous_knob->x=output_level_headroom_knob->x;
    output_level_RMS_knob->x=output_level_headroom_knob->x;
    output_level_headroom_knob->x=output_level_headroom_knob->x;
  }
  last_output_level_PUNCHED_UP_TO_THE_MAX_knob_x=output_level_PUNCHED_UP_TO_THE_MAX_knob->x;
  last_output_level_instantaneous_knob_x        =output_level_instantaneous_knob->x;
  last_output_level_RMS_knob_x                  =output_level_RMS_knob->x;
  last_output_level_headroom_knob_x             =output_level_headroom_knob->x;

//  float desired_knob_value;
//  switch(normalized_rate_to_track_parameter_changes_knob->displayed_frame){
//  case 0 :desired_knob_value=00.003f;break;
//  case 1 :desired_knob_value=00.01f;break;
//  case 2 :desired_knob_value=00.02f;break;
//  case 3 :desired_knob_value=00.03f;break;
//  case 4 :desired_knob_value=00.04f;break;
//  case 5 :desired_knob_value=00.05f;break;
//  case 6 :desired_knob_value=00.06f;break;
//  case 7 :desired_knob_value=00.08f;break;
//  case 8 :desired_knob_value=00.10f;break;
//  case 9 :desired_knob_value=00.13f;break;
//  case 10:desired_knob_value=00.16f;break;
//  case 11:desired_knob_value=00.20f;break;
//  case 12:desired_knob_value=00.25f;break;
//  case 13:desired_knob_value=00.32f;break;
//  case 14:desired_knob_value=00.40f;break;
//  case 15:desired_knob_value=00.50f;break;
//  case 16:desired_knob_value=00.63f;break;
//  case 17:desired_knob_value=00.80f;break;
//  case 18:desired_knob_value=01.00f;break;
//  case 19:desired_knob_value=01.25f;break;
//  case 20:desired_knob_value=01.60f;break;
//  case 21:desired_knob_value=02.00f;break;
//  case 22:desired_knob_value=03.15f;break;
//  case 23:desired_knob_value=04.00f;break;
//  case 24:desired_knob_value=05.00f;break;
//  case 25:desired_knob_value=06.30f;break;
//  case 26:desired_knob_value=08.00f;break;
//  case 27:desired_knob_value=10.00f;break;
//  case 28:desired_knob_value=12.50f;break;
//  case 29:desired_knob_value=16.00f;break;
//  case 30:desired_knob_value=20.00f;break;
//  case 31:desired_knob_value=31.50f;break;
//  case 32:desired_knob_value=40.00f;break;
//  case 33:desired_knob_value=50.00f;break;
//  case 34:desired_knob_value=63.00f;break;
//  case 35:desired_knob_value=80.00f;break;
//  case 36:desired_knob_value=99.99f;break;
//  }

  static float meat_buttes=0;
  float mb2=phase_meater_zoom_rate_normalized*phase_meater_x_zoom*phase_meater_update_rate*phase_meater_master_zoom;
  if (meat_buttes!=mb2){
    noco->m(hw,"phase meter changed");
    meat->show_info=true;
    SetTimer(hw,TIMER_EVENT_PHASE_METER_SHO_INFO,20000,0);
  }
  meat_buttes=mb2;
  meat->zet_zoom_rate(phase_meater_zoom_rate_normalized);
  meat->aspect_ratio=1.0f+2.0f*(1.0f-phase_meater_x_zoom);
  meat->fit_to_this_fraction_of_display=0.1f+0.9f*phase_meater_master_zoom;
  static float old_meat_phase_meater_update_rate=phase_meater_update_rate;
  if (old_meat_phase_meater_update_rate!=phase_meater_update_rate){
    if (phase_meater_update_rate<0.51f){
      float f=0.5f-phase_meater_update_rate;
      float extra=powf(f,2.9f)*8000.0f;
      int ms=(int)(10.0f+extra+0.5f);
      meat->update_rate=(float)ms;
      SetTimer(hw,TIMER_EVENT_PHASE_METER_UPDATE_DATA,ms,0);
      ms=min(ms,20);
      SetTimer(hw,TIMER_EVENT_PHASE_METER_DRAW,ms,0);
      KillTimer(hw,TIMER_EVENT_PHASE_METER_PAUSE);
      pause_phase_meter_update=false;
    }else{
      SetTimer(hw,TIMER_EVENT_PHASE_METER_PAUSE,(int)(200.0f+(phase_meater_update_rate-0.5f)*4000.0f),0);
      SetTimer(hw,TIMER_EVENT_PHASE_METER_UPDATE_DATA,20,0);
      SetTimer(hw,TIMER_EVENT_PHASE_METER_DRAW,20,0);
      meat->update_rate=20;
    }

  }
  old_meat_phase_meater_update_rate=phase_meater_update_rate;

  bass_channel_compressor_threshold_dB=-40.0f+60.0f*bass_channel_compressor_threshold_normalized;
  bass_channel_compressor_softness=compress_her_limit_her::compressor_knee_radius[bass_channel_compressor_softness_knob->displayed_frame];
  bass_channel_compressor_attack=compress_her_limit_her::attack_time[bass_channel_compressor_attack_knob->displayed_frame];
  bass_channel_compressor_low_release=compress_her_limit_her::release_time[bass_channel_compressor_release_knob->displayed_frame];
  bass_channel_compressor_ratio=compress_her_limit_her::ratios[bass_channel_compressor_ratio_knob->displayed_frame];
  bass_channel_limiter_softness=compress_her_limit_her::compressor_knee_radius[bass_channel_limiter_softness_knob->displayed_frame];
  bass_channel_limiter_threshold_dB=-20.0f+40.0f*bass_channel_limiter_threshold_normalized;
  bass_channel_compressor_output=bass_channel_compressor_output_normalized*20.0f-5.0f;

  bass_her_compressor_threshold_dB=-40.0f+60.0f*bass_her_compressor_threshold_normalized;
  bass_her_compressor_softness=compress_her_limit_her::compressor_knee_radius[bass_her_compressor_softness_knob->displayed_frame];
  bass_her_compressor_attack=compress_her_limit_her::attack_time[bass_her_compressor_attack_knob->displayed_frame];
  bass_her_compressor_low_release=compress_her_limit_her::release_time[bass_her_compressor_release_knob->displayed_frame];
  bass_her_compressor_ratio=compress_her_limit_her::ratios[bass_her_compressor_ratio_knob->displayed_frame];
  bass_her_limiter_softness=compress_her_limit_her::compressor_knee_radius[bass_her_limiter_softness_knob->displayed_frame];
  bass_her_limiter_threshold_dB=-20.0f+40.0f*bass_her_limiter_threshold_normalized;
  bass_her_compressor_output=bass_her_compressor_output_normalized*20.0f-5.0f;

  output_compressor_threshold_dB=-40.0f+60.0f*output_compressor_threshold_normalized;
  output_compressor_softness=compress_her_limit_her::compressor_knee_radius[output_compressor_softness_knob->displayed_frame];
  output_compressor_attack=compress_her_limit_her::attack_time[output_compressor_attack_knob->displayed_frame];
  output_compressor_low_release=compress_her_limit_her::release_time[output_compressor_release_knob->displayed_frame];
  output_compressor_ratio=compress_her_limit_her::ratios[output_compressor_ratio_knob->displayed_frame];
  output_limiter_softness=compress_her_limit_her::compressor_knee_radius[output_limiter_softness_knob->displayed_frame];
  output_limiter_threshold_dB=-20.0f+40.0f*output_limiter_threshold_normalized;
  output_compressor_output=output_compressor_output_normalized*20.0f-5.0f;

  final_compressor_threshold_dB=-40.0f+60.0f*final_compressor_threshold_normalized;
  final_compressor_softness=compress_her_limit_her::compressor_knee_radius[final_compressor_softness_knob->displayed_frame];
  final_compressor_attack=compress_her_limit_her::attack_time[final_compressor_attack_knob->displayed_frame];
  final_compressor_low_release=compress_her_limit_her::release_time[final_compressor_release_knob->displayed_frame];
  final_compressor_ratio=compress_her_limit_her::ratios[final_compressor_ratio_knob->displayed_frame];
  final_limiter_softness=compress_her_limit_her::compressor_knee_radius[final_limiter_softness_knob->displayed_frame];
  final_limiter_threshold_dB=-20.0f+40.0f*final_limiter_threshold_normalized;
  final_compressor_output=final_compressor_output_normalized*20.0f-5.0f;

  if (compress_her_bass){
    static float old_bass_channel_compressor_threshold_dB=0;
    static float old_bass_channel_compressor_softness=    0;
    static float old_bass_channel_compressor_ratio=       0;
    static float old_bass_channel_limiter_softness=       0;
    static float old_bass_channel_limiter_threshold_dB=   0;
    compress_her_bass->needs_redraw=old_bass_channel_compressor_threshold_dB!=bass_channel_compressor_threshold_dB;
    compress_her_bass->needs_redraw|=old_bass_channel_compressor_softness!=    bass_channel_compressor_softness;
    compress_her_bass->needs_redraw|=old_bass_channel_compressor_ratio!=       bass_channel_compressor_ratio;
    compress_her_bass->needs_redraw|=old_bass_channel_limiter_softness!=       bass_channel_limiter_softness;
    compress_her_bass->needs_redraw|=old_bass_channel_limiter_threshold_dB!=   bass_channel_limiter_threshold_dB;
    need_redraw_bass_channel_compressor|=compress_her_bass->needs_redraw;

    old_bass_channel_compressor_threshold_dB=bass_channel_compressor_threshold_dB;
    old_bass_channel_compressor_softness=    bass_channel_compressor_softness;
    old_bass_channel_compressor_ratio=       bass_channel_compressor_ratio;
    old_bass_channel_limiter_softness=       bass_channel_limiter_softness;
    old_bass_channel_limiter_threshold_dB=   bass_channel_limiter_threshold_dB;
    compress_her_bass->LED_rect=Crectal((int)bass_channel_dummy_for_compressor_LEDs->x,(int)bass_channel_dummy_for_compressor_LEDs->y,(int)bass_channel_dummy_for_compressor_LEDs->x+(int)bass_channel_dummy_for_compressor_LEDs->width,(int)bass_channel_dummy_for_compressor_LEDs->y+(int)bass_channel_dummy_for_compressor_LEDs->height);
    if (bass_channel_compressor_bypass_knob_global->displayed_frame!=0){
      compress_her_bass->bypass=true;
    }else{
      compress_her_bass->bypass=false;
    }
  }
  if (compress_her_ssb){
    static float old_bass_her_compressor_threshold_dB=0;
    static float old_bass_her_compressor_softness=    0;
    static float old_bass_her_compressor_ratio=       0;
    static float old_bass_her_limiter_softness=       0;
    static float old_bass_her_limiter_threshold_dB=   0;
    compress_her_ssb->needs_redraw=old_bass_her_compressor_threshold_dB!=bass_her_compressor_threshold_dB;
    compress_her_ssb->needs_redraw|=old_bass_her_compressor_softness!=    bass_her_compressor_softness;
    compress_her_ssb->needs_redraw|=old_bass_her_compressor_ratio!=       bass_her_compressor_ratio;
    compress_her_ssb->needs_redraw|=old_bass_her_limiter_softness!=       bass_her_limiter_softness;
    compress_her_ssb->needs_redraw|=old_bass_her_limiter_threshold_dB!=   bass_her_limiter_threshold_dB;
    need_redraw_bass_her_compressor|=compress_her_ssb->needs_redraw;

    old_bass_her_compressor_threshold_dB=bass_her_compressor_threshold_dB;
    old_bass_her_compressor_softness=    bass_her_compressor_softness;
    old_bass_her_compressor_ratio=       bass_her_compressor_ratio;
    old_bass_her_limiter_softness=       bass_her_limiter_softness;
    old_bass_her_limiter_threshold_dB=   bass_her_limiter_threshold_dB;
    compress_her_ssb->LED_rect=Crectal((int)bass_her_dummy_for_compressor_LEDs->x,(int)bass_her_dummy_for_compressor_LEDs->y,(int)bass_her_dummy_for_compressor_LEDs->x+(int)bass_her_dummy_for_compressor_LEDs->width,(int)bass_her_dummy_for_compressor_LEDs->y+(int)bass_her_dummy_for_compressor_LEDs->height);
    if (bass_her_compressor_bypass_knob_global->displayed_frame!=0){
      compress_her_ssb->bypass=true;
    }else{
      compress_her_ssb->bypass=false;
    }
  }
  if (compress_her_out){
    static float old_output_compressor_threshold_dB=0;
    static float old_output_compressor_softness=    0;
    static float old_output_compressor_ratio=       0;
    static float old_output_limiter_softness=       0;
    static float old_output_limiter_threshold_dB=   0;
    compress_her_out->needs_redraw=old_output_compressor_threshold_dB!=output_compressor_threshold_dB;
    compress_her_out->needs_redraw|=old_output_compressor_softness!=    output_compressor_softness;
    compress_her_out->needs_redraw|=old_output_compressor_ratio!=       output_compressor_ratio;
    compress_her_out->needs_redraw|=old_output_limiter_softness!=       output_limiter_softness;
    compress_her_out->needs_redraw|=old_output_limiter_threshold_dB!=   output_limiter_threshold_dB;
    need_redraw_output_compressor|=compress_her_out->needs_redraw;

    old_output_compressor_threshold_dB=output_compressor_threshold_dB;
    old_output_compressor_softness=    output_compressor_softness;
    old_output_compressor_ratio=       output_compressor_ratio;
    old_output_limiter_softness=       output_limiter_softness;
    old_output_limiter_threshold_dB=   output_limiter_threshold_dB;
    compress_her_out->LED_rect=Crectal((int)output_dummy_for_compressor_LEDs->x,(int)output_dummy_for_compressor_LEDs->y,(int)output_dummy_for_compressor_LEDs->x+(int)output_dummy_for_compressor_LEDs->width,(int)output_dummy_for_compressor_LEDs->y+(int)output_dummy_for_compressor_LEDs->height);
    if (output_compressor_bypass_knob_global->displayed_frame!=0){
      compress_her_out->bypass=true;
    }else{
      compress_her_out->bypass=false;
    }
  }
  if (compress_her_final){
    static float old_final_compressor_threshold_dB=0;
    static float old_final_compressor_softness=    0;
    static float old_final_compressor_ratio=       0;
    static float old_final_limiter_softness=       0;
    static float old_final_limiter_threshold_dB=   0;
    compress_her_final->needs_redraw=old_final_compressor_threshold_dB!=final_compressor_threshold_dB;
    compress_her_final->needs_redraw|=old_final_compressor_softness!=    final_compressor_softness;
    compress_her_final->needs_redraw|=old_final_compressor_ratio!=       final_compressor_ratio;
    compress_her_final->needs_redraw|=old_final_limiter_softness!=       final_limiter_softness;
    compress_her_final->needs_redraw|=old_final_limiter_threshold_dB!=   final_limiter_threshold_dB;
    need_redraw_final_compressor|=compress_her_final->needs_redraw;

    old_final_compressor_threshold_dB=final_compressor_threshold_dB;
    old_final_compressor_softness=    final_compressor_softness;
    old_final_compressor_ratio=       final_compressor_ratio;
    old_final_limiter_softness=       final_limiter_softness;
    old_final_limiter_threshold_dB=   final_limiter_threshold_dB;
    compress_her_final->LED_rect=Crectal((int)final_dummy_for_compressor_LEDs->x,(int)final_dummy_for_compressor_LEDs->y,(int)final_dummy_for_compressor_LEDs->x+(int)final_dummy_for_compressor_LEDs->width,(int)final_dummy_for_compressor_LEDs->y+(int)final_dummy_for_compressor_LEDs->height);
    if (final_compressor_bypass_knob_global->displayed_frame!=0){
      compress_her_final->bypass=true;
    }else{
      compress_her_final->bypass=false;
    }
  }

  if (rtaR && rtaL && bm){
    static int of1=0;
    static int of2=0;
    static int of3=0;
    static int oldgraph_offset_in_dBs =0;
    if (graph_offset_in_dBs!=oldgraph_offset_in_dBs || of1!=dummy_for_RTAR->displayed_frame || of2!=dummy_for_RTAL->displayed_frame || of3!=dummy_for_BASSMON->displayed_frame){
      rtaR->needs_redraw_of_background=true;
      rtaL->needs_redraw_of_background=true;
      bm->needs_redraw_of_background=true;
    }
    of1=dummy_for_RTAR->displayed_frame;
    of2=dummy_for_RTAL->displayed_frame;
    of3=dummy_for_BASSMON->displayed_frame;
    oldgraph_offset_in_dBs=graph_offset_in_dBs;
  }


//  normalized_rate_to_track_parameter_changes=1.0f/(desired_knob_value*44100.0f);
  bass_her_clean_up_lowpass_index=(int)((float)(NUM_BASS_HER_LOWPASS_CLEANUP_FILTERS-1)*bass_her_clean_up_low_pass_index_normalized+0.5f);
  bass_her_clean_up_highpass_index=(int)((float)(NUM_BASS_HER_HIGHPASS_CLEANUP_FILTERS-1)*bass_her_clean_up_highpass_index_normalized+0.5f);
  weaver_ssb_centre_low_pass_filter_index=(int)((float)(NUM_WEAVER_SSB_CENTRE_LOWPASS_FILTERS-1)*weaver_ssb_centre_low_pass_filter_index_normalized+0.5f);
  bassher.weaver_ssb_carrier_frequency=bass_her_weaver_ssb_carrier_frequency_normalized*(WEAVER_SSB_CARRIER_FREQUENCY_END-WEAVER_SSB_CARRIER_FREQUENCY_START)+WEAVER_SSB_CARRIER_FREQUENCY_START;
  bassher.weaver_bandwidth=bass_her_weaver_ssb_bandwidth_normalized*(WEAVER_SSB_BANDWIDTH_END-WEAVER_SSB_BANDWIDTH_START)+WEAVER_SSB_BANDWIDTH_START;
  if (rtaR){
    rtaR->x=(float)dummy_for_RTAL->x;
    rtaR->y=(float)dummy_for_RTAL->y;
  }
  if (rtaL){
    rtaL->x=(float)dummy_for_RTAR->x;
    rtaL->y=(float)dummy_for_RTAR->y;
  }
  if (bm){
    bm->x=(float)dummy_for_BASSMON->x;
    bm->y=(float)dummy_for_BASSMON->y;
  }

  if (bass_her_compressor_lock){
    bass_her_compressor_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_compressor_ratio_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_compressor_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;;
    bass_her_compressor_attack_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_compressor_release_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_limiter_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_limiter_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_dummy_for_compressor_LEDs->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_compressor_bypass_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_compressor_unique_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_her_compressor_output_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
  }else{
    bass_her_compressor_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_compressor_ratio_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_compressor_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;;
    bass_her_compressor_attack_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_compressor_release_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_limiter_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_limiter_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_dummy_for_compressor_LEDs->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_compressor_bypass_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_compressor_unique_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_her_compressor_output_knob->dont_reload_me=RELOAD_FROM_PRESET;
  }
  if (output_compressor_lock){
    output_compressor_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_compressor_ratio_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_compressor_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;;
    output_compressor_attack_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_compressor_release_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_limiter_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_limiter_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_dummy_for_compressor_LEDs->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_compressor_bypass_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_compressor_unique_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    output_compressor_output_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
  }else{
    output_compressor_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_compressor_ratio_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_compressor_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;;
    output_compressor_attack_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_compressor_release_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_limiter_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_limiter_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_dummy_for_compressor_LEDs->dont_reload_me=RELOAD_FROM_PRESET;
    output_compressor_bypass_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_compressor_unique_knob->dont_reload_me=RELOAD_FROM_PRESET;
    output_compressor_output_knob->dont_reload_me=RELOAD_FROM_PRESET;
  }
  if (bass_channel_compressor_lock){
    bass_channel_compressor_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_compressor_ratio_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_compressor_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;;
    bass_channel_compressor_attack_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_compressor_release_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_limiter_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_limiter_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_dummy_for_compressor_LEDs->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_compressor_bypass_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_compressor_unique_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    bass_channel_compressor_output_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
  }else{
    bass_channel_compressor_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_compressor_ratio_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_compressor_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;;
    bass_channel_compressor_attack_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_compressor_release_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_limiter_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_limiter_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_dummy_for_compressor_LEDs->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_compressor_bypass_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_compressor_unique_knob->dont_reload_me=RELOAD_FROM_PRESET;
    bass_channel_compressor_output_knob->dont_reload_me=RELOAD_FROM_PRESET;
  }
  if (final_compressor_lock){
    final_compressor_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_compressor_ratio_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_compressor_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;;
    final_compressor_attack_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_compressor_release_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_limiter_threshold_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_limiter_softness_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_dummy_for_compressor_LEDs->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_compressor_bypass_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_compressor_unique_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
    final_compressor_output_knob->dont_reload_me=DONT_RELOAD_FROM_PRESET;
  }else{
    final_compressor_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_compressor_ratio_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_compressor_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;;
    final_compressor_attack_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_compressor_release_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_limiter_threshold_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_limiter_softness_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_dummy_for_compressor_LEDs->dont_reload_me=RELOAD_FROM_PRESET;
    final_compressor_bypass_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_compressor_unique_knob->dont_reload_me=RELOAD_FROM_PRESET;
    final_compressor_output_knob->dont_reload_me=RELOAD_FROM_PRESET;
  }

  if (compress_her_bass){
    static int last_x=bass_channel_dummy_for_compressor->x;
    static int last_y=bass_channel_dummy_for_compressor->y;
    compress_her_bass->x=(float)bass_channel_dummy_for_compressor->x;
    compress_her_bass->y=(float)bass_channel_dummy_for_compressor->y;
    int dx=bass_channel_dummy_for_compressor->x-last_x;
    int dy=bass_channel_dummy_for_compressor->y-last_y;
    if ((dx!=0 || dy!=0) && GetAsyncKeyState('Z')&0x8000){
      nudge_control(hw,bass_channel_compressor_threshold_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_ratio_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_softness_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_attack_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_release_knob,dx,dy);
      nudge_control(hw,bass_channel_limiter_threshold_knob,dx,dy);
      nudge_control(hw,bass_channel_limiter_softness_knob,dx,dy);
      nudge_control(hw,bass_channel_dummy_for_compressor_LEDs,dx,dy);
      nudge_control(hw,bass_channel_compressor_bypass_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_unique_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_lock_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_output_knob,dx,dy);
      nudge_control(hw,bass_channel_compressor_mute_knob,dx,dy);

    }
    last_x=bass_channel_dummy_for_compressor->x;
    last_y=bass_channel_dummy_for_compressor->y;
  }
  if (compress_her_ssb){
    static int last_x=bass_her_dummy_for_compressor->x;
    static int last_y=bass_her_dummy_for_compressor->y;
    compress_her_ssb->x=(float)bass_her_dummy_for_compressor->x;
    compress_her_ssb->y=(float)bass_her_dummy_for_compressor->y;
    int dx=bass_her_dummy_for_compressor->x-last_x;
    int dy=bass_her_dummy_for_compressor->y-last_y;
    if ((dx!=0 || dy!=0)){
      rttal->do_a_global_reeeeeeefreshhhhh(hw);
      if (GetAsyncKeyState('A')&0x8000){
        bass_channel_dummy_for_compressor->Invalidate_me(hw);
        output_dummy_for_compressor->Invalidate_me(hw);
        bass_channel_dummy_for_compressor->x=bass_her_dummy_for_compressor->x;
        output_dummy_for_compressor->x=bass_her_dummy_for_compressor->x;
      }
      if (GetAsyncKeyState('Z')&0x8000){
        nudge_control(hw,bass_her_compressor_threshold_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_ratio_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_softness_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_attack_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_release_knob,dx,dy);
        nudge_control(hw,bass_her_limiter_threshold_knob,dx,dy);
        nudge_control(hw,bass_her_limiter_softness_knob,dx,dy);
        nudge_control(hw,bass_her_dummy_for_compressor_LEDs,dx,dy);
        nudge_control(hw,bass_her_compressor_bypass_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_unique_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_lock_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_output_knob,dx,dy);
        nudge_control(hw,bass_her_compressor_mute_knob,dx,dy);

        nudge_control_relative(hw,output_compressor_threshold_knob,bass_her_compressor_threshold_knob,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_ratio_knob    ,bass_her_compressor_ratio_knob    ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_softness_knob ,bass_her_compressor_softness_knob ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_attack_knob   ,bass_her_compressor_attack_knob   ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_release_knob  ,bass_her_compressor_release_knob  ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_limiter_threshold_knob   ,bass_her_limiter_threshold_knob   ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_limiter_softness_knob    ,bass_her_limiter_softness_knob    ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_dummy_for_compressor_LEDs,bass_her_dummy_for_compressor_LEDs,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_bypass_knob   ,bass_her_compressor_bypass_knob   ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_unique_knob   ,bass_her_compressor_unique_knob   ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_lock_knob     ,bass_her_compressor_lock_knob     ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_output_knob   ,bass_her_compressor_output_knob   ,output_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,output_compressor_mute_knob     ,bass_her_compressor_mute_knob     ,output_dummy_for_compressor,bass_her_dummy_for_compressor);

        nudge_control_relative(hw,bass_channel_compressor_threshold_knob,bass_her_compressor_threshold_knob,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_ratio_knob    ,bass_her_compressor_ratio_knob    ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_softness_knob ,bass_her_compressor_softness_knob ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_attack_knob   ,bass_her_compressor_attack_knob   ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_release_knob  ,bass_her_compressor_release_knob  ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_limiter_threshold_knob   ,bass_her_limiter_threshold_knob   ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_limiter_softness_knob    ,bass_her_limiter_softness_knob    ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_dummy_for_compressor_LEDs,bass_her_dummy_for_compressor_LEDs,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_bypass_knob   ,bass_her_compressor_bypass_knob   ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_unique_knob   ,bass_her_compressor_unique_knob   ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_lock_knob     ,bass_her_compressor_lock_knob     ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_output_knob   ,bass_her_compressor_output_knob   ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,bass_channel_compressor_mute_knob     ,bass_her_compressor_mute_knob     ,bass_channel_dummy_for_compressor,bass_her_dummy_for_compressor);

        nudge_control_relative(hw,final_compressor_threshold_knob,bass_her_compressor_threshold_knob,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_ratio_knob    ,bass_her_compressor_ratio_knob    ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_softness_knob ,bass_her_compressor_softness_knob ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_attack_knob   ,bass_her_compressor_attack_knob   ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_release_knob  ,bass_her_compressor_release_knob  ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_limiter_threshold_knob   ,bass_her_limiter_threshold_knob   ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_limiter_softness_knob    ,bass_her_limiter_softness_knob    ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_dummy_for_compressor_LEDs,bass_her_dummy_for_compressor_LEDs,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_bypass_knob   ,bass_her_compressor_bypass_knob   ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_unique_knob   ,bass_her_compressor_unique_knob   ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_lock_knob     ,bass_her_compressor_lock_knob     ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_output_knob   ,bass_her_compressor_output_knob   ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
        nudge_control_relative(hw,final_compressor_mute_knob     ,bass_her_compressor_mute_knob     ,final_dummy_for_compressor,bass_her_dummy_for_compressor);
      }
    }
    last_x=bass_her_dummy_for_compressor->x;
    last_y=bass_her_dummy_for_compressor->y;
  }

  if (compress_her_out){
    static int last_x=output_dummy_for_compressor->x;
    static int last_y=output_dummy_for_compressor->y;
    compress_her_out->x=(float)output_dummy_for_compressor->x;
    compress_her_out->y=(float)output_dummy_for_compressor->y;
    int dx=output_dummy_for_compressor->x-last_x;
    int dy=output_dummy_for_compressor->y-last_y;
    if ((dx!=0 || dy!=0) && GetAsyncKeyState('Z')&0x8000){
      nudge_control(hw,output_compressor_threshold_knob,dx,dy);
      nudge_control(hw,output_compressor_ratio_knob,dx,dy);
      nudge_control(hw,output_compressor_softness_knob,dx,dy);
      nudge_control(hw,output_compressor_attack_knob,dx,dy);
      nudge_control(hw,output_compressor_release_knob,dx,dy);
      nudge_control(hw,output_limiter_threshold_knob,dx,dy);
      nudge_control(hw,output_limiter_softness_knob,dx,dy);
      nudge_control(hw,output_dummy_for_compressor_LEDs,dx,dy);
      nudge_control(hw,output_compressor_bypass_knob,dx,dy);
      nudge_control(hw,output_compressor_unique_knob,dx,dy);
      nudge_control(hw,output_compressor_lock_knob,dx,dy);
      nudge_control(hw,output_compressor_output_knob,dx,dy);
      nudge_control(hw,output_compressor_mute_knob,dx,dy);
    }
    last_x=output_dummy_for_compressor->x;
    last_y=output_dummy_for_compressor->y;
  }
  if (compress_her_final){
    static int last_x=final_dummy_for_compressor->x;
    static int last_y=final_dummy_for_compressor->y;
    compress_her_final->x=(float)final_dummy_for_compressor->x;
    compress_her_final->y=(float)final_dummy_for_compressor->y;
    int dx=final_dummy_for_compressor->x-last_x;
    int dy=final_dummy_for_compressor->y-last_y;
    if ((dx!=0 || dy!=0) && GetAsyncKeyState('Z')&0x8000){
      nudge_control(hw,final_compressor_threshold_knob,dx,dy);
      nudge_control(hw,final_compressor_ratio_knob,dx,dy);
      nudge_control(hw,final_compressor_softness_knob,dx,dy);
      nudge_control(hw,final_compressor_attack_knob,dx,dy);
      nudge_control(hw,final_compressor_release_knob,dx,dy);
      nudge_control(hw,final_limiter_threshold_knob,dx,dy);
      nudge_control(hw,final_limiter_softness_knob,dx,dy);
      nudge_control(hw,final_dummy_for_compressor_LEDs,dx,dy);
      nudge_control(hw,final_compressor_bypass_knob,dx,dy);
      nudge_control(hw,final_compressor_unique_knob,dx,dy);
      nudge_control(hw,final_compressor_lock_knob,dx,dy);
      nudge_control(hw,final_compressor_output_knob,dx,dy);
      nudge_control(hw,final_compressor_mute_knob,dx,dy);

    }
    last_x=final_dummy_for_compressor->x;
    last_y=final_dummy_for_compressor->y;
  }

  if (!just_loaded_a_new_preset){
    if (last_bass_her_weaver_ssb_onoff!=bass_her_weaver_ssb_onoff){
      bass_her_pass_only_extra_bass_onoff_knob->set_control_to_this_normalized_float_value(0);
      bass_her_pass_only_extra_bass_onoff_knob->Invalidate_me(hw);
    }
  }
  last_bass_her_weaver_ssb_onoff=bass_her_weaver_ssb_onoff;

  sharpener_light->test_light_state(hw,sharpen_the_nig);
  widener_light->test_light_state(hw,widen_the_nig);
  equalizer_light->test_light_state(hw,equalize_the_nig);
  weaver_ssb_light->test_light_state(hw,bass_her_weaver_ssb_onoff);
  equalizer_AH_light->test_light_state(hw,equalizer_AH_on_or_off);

//  RTTSG_onoff_light->test_light_state(hw,RTTSG_on_or_off);
//  RTTSG_bypass_light->test_light_state(hw,RTTSG_bypass_on_or_off);
  schroeder_on_off_light->test_light_state(hw,schroeder_on_off);
  static float last_RTTSG_on_or_off=-1.0f;
  if (RTTSG_on_or_off!=last_RTTSG_on_or_off){
    if (RTTSG_on_or_off==0.0f){
      final_meat->make_IT_rough();
    }
  }
  last_RTTSG_on_or_off=RTTSG_on_or_off;

  if (dummy_for_RTAL->displayed_frame==CLICKED_WITH_MOUSE_BUTTHOLMES){
    dummy_for_RTAL->displayed_frame=0;
    rtaL->shrink_sidegraph^=true;
  }
  if (dummy_for_RTAR->displayed_frame==CLICKED_WITH_MOUSE_BUTTHOLMES){
    dummy_for_RTAR->displayed_frame=0;
    rtaR->shrink_sidegraph^=true;
  }
  if (dummy_for_BASSMON->displayed_frame==CLICKED_WITH_MOUSE_BUTTHOLMES){
    dummy_for_BASSMON->displayed_frame=0;
    bm->shrink_sidegraph^=true;
  }

  do_rta_radio(hw);

  if (save_preset_radio){
    save_preset_radio=0;
    int f=schnurtgren_guibertube_selector_knob->displayed_frame;
    assert(0);
    dump_preset2(hw,f);
    noco->m(hw,("DUMPED PRESET"+lexical_cast<string>(f)).data());
  }
  if (undo_radio){
    undo_radio=0;
    undo(hw);
    noco->m(hw,"UNDO LEVEL");
  }
  if (redo_radio){
    redo_radio=0;
    redo(hw);
    noco->m(hw,"REDO LEVEL");
  }
//  if (UI_layout_preset_load){
//    UI_layout_preset_load=0;
//    dump_ui_config(0);
//    InvalidateRect(hw,0,true);
//    noco->m(hw,"Saved UI config");
//  }
//  if (UI_layout_preset_save){
//    UI_layout_preset_save=0;
//    load_ui_config(0);
//    InvalidateRect(hw,0,true);
//    noco->m(hw,"Loaded UI config");
//  }
//  if (UI_layout_preset_default){
//    UI_layout_preset_default=0;
//    load_ui_config(99);
//    InvalidateRect(hw,0,true);
//    noco->m(hw,"Loaded DEFAULT UI config");
//  }
  just_loaded_a_new_preset=false;

  static float old_weighting_curve_float=weighting_curve_float;
  if (old_weighting_curve_float!=weighting_curve_float){
    rtaL->needs_redraw_of_background=true;
    rtaR->needs_redraw_of_background=true;
    bm->needs_redraw_of_background=true;
  }

  dbs_of_big_knob_wants_to_be=output_gain_normalized*20.0f;
  static float last_dbs_of_big_knob=-1;
  if (compress_her_final){
    if (dbs_of_big_knob_wants_to_be!=last_dbs_of_big_knob){
      adjust_final_output_limiter_threshold_for_output_gain_control(hw);
    }
  }
  last_dbs_of_big_knob=dbs_of_big_knob_wants_to_be;

  if (meat){
    meat->x=(int)dummy_for_face_meat_her_knob->x;
    meat->y=(int)dummy_for_face_meat_her_knob->y;
    meat->width=(int)dummy_for_face_meat_her_knob->width;
    meat->height=(int)dummy_for_face_meat_her_knob->height;
  }
  if (meat){
    final_meat->x=(int)dummy_for_output_meter_knob->x;
    final_meat->y=(int)dummy_for_output_meter_knob->y;
    final_meat->width=(int)dummy_for_output_meter_knob->width;
    final_meat->height=(int)dummy_for_output_meter_knob->height;
  }
  if (crest_factor_meat){
    crest_factor_meat->x=(int)dummy_for_crest_factor_meter_knob->x;
    crest_factor_meat->y=(int)dummy_for_crest_factor_meter_knob->y;
    crest_factor_meat->width=(int)dummy_for_crest_factor_meter_knob->width;
    crest_factor_meat->height=(int)dummy_for_crest_factor_meter_knob->height;
  }
  //  static float last_final_limiter_softness_normalized=0;
  //  if (last_final_limiter_softness_normalized!=final_limiter_softness_normalized){
  //    SetTimer(hw,TIMER_EVENT_REDRAW_FINAL_COMPRESSOR_GRAPHIC_TO_FIX_REDRAW_BUG,50,0);
  //    last_final_limiter_softness_normalized=final_limiter_softness_normalized;
  //  }
  set_processing_parameters_from_global_settings();
}




void processing::do_some( float **inputs, float **outputs, int num){
  set_processing_parameters_from_global_settings();
  for(int i=0;i<num;i++){
    float l=inputs[0][i];
    float r=inputs[1][i];
    doit(l,r);
    outputs[0][i]=l;
    outputs[1][i]=r;
  }
}





void processing::set_processing_parameters_from_global_settings(){
  widener.set_global_parameters(global_bas_gain_wants_to_be,global_mid_gain_wants_to_be,global_top_gain_wants_to_be,global_bas_width,global_mid_width,global_top_width, equalize_the_nig,widen_the_nig);
  sharpenL.set_global_parameters(global_sharpen_ammount, sharpen_the_nig);
  sharpenR.set_global_parameters(global_sharpen_ammount, sharpen_the_nig);
  bassher.set_global_parameters(bass_her_pass_only_extra_bass_onoff, bass_her_ammount_of_bass_boost_wants_to_be, bass_her_weaver_ssb_onoff);
  if (compress_her_bass){
    compress_her_bass->set_global_parameters_low(bass_channel_compressor_threshold_dB,bass_channel_compressor_softness,bass_channel_compressor_ratio,bass_channel_compressor_attack,bass_channel_compressor_low_release,bass_channel_limiter_threshold_dB,bass_channel_limiter_softness,bass_channel_compressor_output);
  }
  if (compress_her_ssb){
    compress_her_ssb->set_global_parameters_low(bass_her_compressor_threshold_dB,bass_her_compressor_softness,bass_her_compressor_ratio,bass_her_compressor_attack,bass_her_compressor_low_release,bass_her_limiter_threshold_dB,bass_her_limiter_softness,bass_her_compressor_output);
  }
  if (compress_her_out){
    compress_her_out->set_global_parameters_low(output_compressor_threshold_dB,output_compressor_softness,output_compressor_ratio,output_compressor_attack,output_compressor_low_release,output_limiter_threshold_dB,output_limiter_softness,output_compressor_output);
  }
  if (compress_her_final){
    compress_her_final->set_global_parameters_low(final_compressor_threshold_dB,final_compressor_softness,final_compressor_ratio,final_compressor_attack,final_compressor_low_release,final_limiter_threshold_dB,final_limiter_softness,final_compressor_output,true);
  }
}







void processing::set_schroeder_reverb_UI_knobz(){
  schroeder_comb_delay1_normalized=global_reverb.get_comb_delay(0);
  schroeder_comb_delay2_normalized=global_reverb.get_comb_delay(1);
  schroeder_comb_delay3_normalized=global_reverb.get_comb_delay(2);
  schroeder_comb_delay4_normalized=global_reverb.get_comb_delay(3);
  schroeder_allpass_delay1_normalized=global_reverb.get_allpass_delay(0);
  schroeder_allpass_delay2_normalized=global_reverb.get_allpass_delay(1);
  schroeder_allpass_reverb1_normalized=global_reverb.get_allpass_reverb(0);
  schroeder_allpass_reverb2_normalized=global_reverb.get_allpass_reverb(1);
  schroeder_reverberation_time_normalized=global_reverb.get_reverberation_time();
  schroeder_comb_low_pass_normalized=global_reverb.get_comb_filter_low_pass_frequency();
  schroeder_combs0_7_buffer_length_normalized=global_reverb.get_combs_5_12_buffer_length_modifier_override();

  schroeder_comb_delay1_knob->set_control_to_this_normalized_float_value(schroeder_comb_delay1_normalized);
  schroeder_comb_delay2_knob->set_control_to_this_normalized_float_value(schroeder_comb_delay2_normalized);
  schroeder_comb_delay3_knob->set_control_to_this_normalized_float_value(schroeder_comb_delay3_normalized);
  schroeder_comb_delay4_knob->set_control_to_this_normalized_float_value(schroeder_comb_delay4_normalized);
  schroeder_allpass_delay1_knob->set_control_to_this_normalized_float_value(schroeder_allpass_delay1_normalized);
  schroeder_allpass_delay2_knob->set_control_to_this_normalized_float_value(schroeder_allpass_delay2_normalized);
  schroeder_allpass_reverb1_knob->set_control_to_this_normalized_float_value(schroeder_allpass_reverb1_normalized);
  schroeder_allpass_reverb2_knob->set_control_to_this_normalized_float_value(schroeder_allpass_reverb2_normalized);
  schroeder_reverberation_time_knob->set_control_to_this_normalized_float_value(schroeder_reverberation_time_normalized);
  schroeder_combs0_7_buffer_length_knob->set_control_to_this_normalized_float_value(schroeder_combs0_7_buffer_length_normalized);
  schroeder_low_pass_knob->set_control_to_this_normalized_float_value(schroeder_comb_low_pass_normalized);
}






control_pic* processing::add_dial_multi_image(vector<string> &images,int _x,int _y,int _frame,float _degrees_start, float _degrees_end,float *_the_float_to_change_to_reflect_current_setting,char*_group,char*_desc,bool _dont_reload_me){
  char temp_group[1024];
  strcpy_s(temp_group,1024,global_channel_name_prefix.data());
  strcat_s(temp_group,1024,"____");
  strcat_s(temp_group,1024,_group);
  control_pic*cp=new control_pic(images,_x,_y,_frame,_degrees_start,_degrees_end,_the_float_to_change_to_reflect_current_setting,temp_group,_desc,_dont_reload_me);
  knobs.push_back(cp);
  return cp;
}



control_pic* processing::add_dial(char*name,int _x,int _y,int _frame,float _degrees_start, float _degrees_end,float *_the_float_to_change_to_reflect_current_setting,char*_group,char*_desc,bool _dont_reload_me){
  vector<string> images;
  images.push_back(string(name));
  char temp_group[1024];
  strcpy_s(temp_group,1024,global_channel_name_prefix.data());
  strcat_s(temp_group,1024,"____");
  strcat_s(temp_group,1024,_group);
  control_pic*cp=new control_pic(images,_x,_y,_frame,_degrees_start,_degrees_end,_the_float_to_change_to_reflect_current_setting,temp_group,_desc,_dont_reload_me);
  knobs.push_back(cp);
  return cp;
}



control_pic* processing::add_switch(char*name,int _x,int _y,float *_the_bool_to_change_to_reflect_current_setting,const char*_group,const char*_desc,bool is_momentary,bool _is_flashing,bool _dont_reload_me){
  vector<string> images;
  images.push_back(string(name));
  char temp_group[1024];
  strcpy_s(temp_group,1024,global_channel_name_prefix.data());
  strcat_s(temp_group,1024,"____");
  strcat_s(temp_group,1024,_group);
  control_pic*cp=new control_pic(images,_x,_y,_the_bool_to_change_to_reflect_current_setting,temp_group,_desc,is_momentary,_is_flashing,_dont_reload_me);
  knobs.push_back(cp);
  return cp;
}



control_pic* processing::add_light(char*name,int _x,int _y,char*_group,char*_desc){
//  char temp_group[1024];
//  strcpy_s(temp_group,1024,global_channel_name_prefix.data());
//  strcat_s(temp_group,1024,"____");
//  strcat_s(temp_group,1024,_group);
  return add_switch(name,_x,_y,0,_group,_desc,IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
}



void processing::check_radio_group(HWND hw,control_pic *c){
  int rg=c->radio_group;
  if(rg){
    for(int i=0;i<(int)knobs.size();i++){
      control_pic *k=knobs[i];
      if (c!=k){
        if (k->radio_group==rg){
          k->displayed_frame=0;
          k->Invalidate_me(hw);
        }
      }
    }
  }
}




void processing::OnLButtonDown(HWND hwnd,UINT nFlags, POINT point) {
  if (GetAsyncKeyState('H')&0x8000){
    for(int i=0;i<(int)knobs.size();i++){
      control_pic *k=knobs[i];
      bool is_hidden=k->hidden&hide_extended_controls;
      if (!is_hidden && k->is_pt_inside_control_rectangular_region_influence_not_the_control_specific_shape_are_we_clear_on_that(&point)){
        controls_to_hide_to_toggle_minimal_UI.insert(k->hash_name());
        recalc_hidden_controls();
        save_knobs_to_hide_for_minimal_ui();
        noco->m(hwnd,("added control to hide"+k->hash_name()).data());
        return;
      }
    }
  }
  left_buttholmes_down=true;
  int moved=clicked_on_something(hwnd,&point);
  check_UI_movement_things(hwnd);
}



int processing::clicked_on_something(HWND hw,POINT *point) {
  int last_set_in_motion=-1;
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    bool is_hidden=k->hidden&hide_extended_controls;
    if (!is_hidden){
      if (k->test_clicked_point(hw,point)){
        check_radio_group(hw,k);
        last_set_in_motion=i;
      }
      if (k->set_position_or_drag_control(hw,point)){
        redraw_unredraw_with_invalidate_things(hw);
      }
    }
  }
  return last_set_in_motion;
}



void processing::redraw_unredraw_with_invalidate_things(HWND hw){
  need_redraw_bass_channel_compressor=true;
  need_redraw_bass_her_compressor=true;
  need_redraw_output_compressor=true;
  SetTimer(hw,TIMER_EVENT_REDRAW_FINAL_COMPRESSOR_GRAPHIC_TO_FIX_REDRAW_BUG,50,0);
  need_redraw_final_compressor=true;
}





void processing::OnRButtonDown(HWND hw,UINT nFlags, POINT point) {
  bool something_pressed=false;
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    bool is_hidden=k->hidden&hide_extended_controls;
    if (!is_hidden){
      if (k->test_clicked_point(hw,&point)){
        k->momentary_switch_press(hw);
        something_pressed=true;
      }
    }
  }
  if (something_pressed){
    check_UI_movement_things(hw);
  }
}



void processing::OnMButtonDown(HWND hwnd,UINT nFlags, POINT point) {
}




void processing::stop_moving_any_seleced_controls(){
  bool save_undo_level=false;
  for(int i=0;i<(int)knobs.size();i++){
    save_undo_level|=knobs[i]->stop_varying_control();
  }
  if (save_undo_level){
    create_undo_level();
  }
}





void processing::OnLButtonUp(HWND hwnd,UINT nFlags, POINT point) {
  left_buttholmes_down=false;
  stop_moving_any_seleced_controls();
  check_UI_movement_things(hwnd);
  for(int i=0;i<(int)momentary_switches.size();i++){
    momentary_switches[i]->unset_momentary_switch(hwnd,true);
  }
}






BOOL processing::OnMouseWheel(HWND hwnd,UINT nState, short aShort, POINT point){
  ScreenToClient(hwnd,&point);
  bool save_undo_level=false;
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    bool is_hidden=k->hidden&hide_extended_controls;
    if (!is_hidden){
      save_undo_level|=knobs[i]->handle_mouse_wheel(aShort,&point,hwnd);
    }
  }
  if (save_undo_level){
    create_undo_level();
  }
  check_UI_movement_things(hwnd);
  return FALSE;
}




void processing::OnLButtonDblClk(HWND hwnd,UINT ui, POINT cp){
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    bool is_hidden=k->hidden&hide_extended_controls;
    if (!is_hidden && k->is_pt_inside_control_rectangular_region_influence_not_the_control_specific_shape_are_we_clear_on_that(&cp)){
      if (!k->test_doubleclick(&cp,hwnd)){
        OnLButtonUp(hwnd,ui,cp);
        OnLButtonDown(hwnd,ui,cp);
      }
    }
  }
}




void processing::recalc_hidden_controls(){
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    if (controls_to_hide_to_toggle_minimal_UI.find(k->hash_name())!=controls_to_hide_to_toggle_minimal_UI.end()){
      k->hidden=true;
    }else{
      k->hidden=false;
    }
  }
//  if (controls_to_hide_to_toggle_minimal_UI.find(
}





void processing::OnDraw(HDC dc,HWND hw,RECT&ur){
  called_from_inside_windows_paint_function_or_must_redraw=true;
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    bool is_hidden=k->hidden&hide_extended_controls;
    if (k->test_rect_for_overlap(ur) && !is_hidden){
      if (k==dummy_for_output_led_output_meters_knob){
        RTTAL::SetDCPenColor2(dc,grey15_cr);
        RTTAL::SetDCBrushColor2(dc,grey15_cr);
        Rectangle(dc,(int)dummy_for_output_led_output_meters_knob->x,(int)dummy_for_output_led_output_meters_knob->y,(int)dummy_for_output_led_output_meters_knob->x+(int)dummy_for_output_led_output_meters_knob->width,(int)dummy_for_output_led_output_meters_knob->y+(int)dummy_for_output_led_output_meters_knob->height);
        int x=(int)dummy_for_output_led_output_meters_knob->x+(((int)dummy_for_output_led_output_meters_knob->width)>>1);
        int y=(int)dummy_for_output_led_output_meters_knob->y+2;
        print_colour_centered(dc,x,y,"PEAK",grey45_cr,true);
        y+=38;
        print_colour_centered(dc,x,y,"CURRENT",grey45_cr,true);
        y+=39;
        print_colour_centered(dc,x,y,"RMS",grey45_cr,true);
        y+=43;
        print_colour_centered(dc,x,y,"HEADROOM",grey45_cr,true);
      }else{
        knobs[i]->plot(dc,hw);
      }
    }
  }
  if (compress_her_ssb){
    if (compress_her_ssb->test_rect_for_overlap(ur)){
      compress_her_ssb->draw(dc);
    }
  }
  if (compress_her_bass){
    if (compress_her_bass->test_rect_for_overlap(ur)){
      compress_her_bass->draw(dc);
    }
  }
  if (compress_her_final){
    if (compress_her_final->test_rect_for_overlap(ur)){
      compress_her_final->draw(dc);
    }
  }
  if (compress_her_out){
    if (compress_her_out->test_rect_for_overlap(ur)){
      compress_her_out->draw(dc);
    }
  }
  SetTextColor(dc,slateblue_cr);
//  string rtt=lexical_cast<string>(renders_last_1_second)+"/"+lexical_cast<string>(timer_render_updates_last_1_second)+" FPS";
//  print_colour(dc,1,1,rtt.data(),grey65_cr);
//  renders++;
  if (my_noco){
    my_noco->x=(float)dummy_for_console_knob->x;
    my_noco->y=(float)dummy_for_console_knob->y;
    //    noco->draw(dc,hw);
    my_noco->draw(dc,hw);
  }
  if (xwaz_console){
    xwaz_console->x=(float)dummy_for_xwaz_display_knob->x;
    xwaz_console->y=(float)dummy_for_xwaz_display_knob->y;
    //    noco->draw(dc,hw);
    xwaz_console->draw(dc,hw,(float)fplayer->current_sample_track_time);
  }
  
  called_from_inside_windows_paint_function_or_must_redraw=false;
}







void processing::OnTimer(HWND hw,UINT nIDEvent){
  switch(nIDEvent){
    //    case TIMER_READ_EXCURSION_MONITOR_DATA:
    //      get_data_from_speaker();
    //      break;
  case TIMER_EVENT_REDRAW_FINAL_COMPRESSOR_GRAPHIC_TO_FIX_REDRAW_BUG:
    // redraw shortly after to allow for any locked out(delayed) user changes to be correctly reflected in the graphic
    need_redraw_final_compressor=true;
    compress_her_final->needs_redraw=true;
//    KillTimer(hw,TIMER_EVENT_REDRAW_FINAL_COMPRESSOR_GRAPHIC_TO_FIX_REDRAW_BUG);
  case TIMER_EVENT_UPDATE_RTA:
    {
//      updates++;
      rtaR->needs_recalc_of_energies=true;
      rtaL->needs_recalc_of_energies=true;
      HDC dc=GetDC(hw);
      string dik=lexical_cast<string>(compress_her_final->gain);
      //        BitBlt(dc,200,200,100,10,0,0,0,BLACKNESS);
      //        print_colour(dc,200,200,dik.data(),white_cr);
      bool is_hidden=dummy_for_RTAR->hidden&hide_extended_controls;
      if (rtaR && !is_hidden){
        rtaR->draw(dc,graph_offset_in_dBs,rta_shows_peaks!=0,true,bm);
      }
      is_hidden=dummy_for_RTAL->hidden&hide_extended_controls;
      if (rtaL && !is_hidden){
        rtaL->draw(dc,graph_offset_in_dBs,rta_shows_peaks!=0,true,bm);
      }
      if (compress_her_bass){
        compress_her_bass->x=(float)bass_channel_dummy_for_compressor->x;
        compress_her_bass->y=(float)bass_channel_dummy_for_compressor->y;
        if (need_redraw_bass_channel_compressor){
          if (compress_her_bass->needs_redraw){
            //              compress_her_bass->desc=bass_channel_dummy_for_compressor->desc.data();
            compress_her_bass->draw_into_cached_bitmap(dc);
          }
          compress_her_bass->draw_compressor_pic_from_cached(dc);
          need_redraw_bass_channel_compressor=false;
        }
        compress_her_bass->draw_LEDs_from_cached(dc);
      }
      if (compress_her_ssb){
        compress_her_ssb->x=(float)bass_her_dummy_for_compressor->x;
        compress_her_ssb->y=(float)bass_her_dummy_for_compressor->y;
        if (need_redraw_bass_her_compressor){
          if (compress_her_ssb->needs_redraw){
            //              compress_her_ssb->desc=bass_her_dummy_for_compressor->desc.data();
            compress_her_ssb->draw_into_cached_bitmap(dc);
          }
          compress_her_ssb->draw_compressor_pic_from_cached(dc);
          need_redraw_bass_her_compressor=false;
        }
        compress_her_ssb->draw_LEDs_from_cached(dc);
      }
      if (compress_her_out){
        compress_her_out->x=(float)output_dummy_for_compressor->x;
        compress_her_out->y=(float)output_dummy_for_compressor->y;
        if (need_redraw_output_compressor){
          if (compress_her_out->needs_redraw){
            //              compress_her_out->desc=output_dummy_for_compressor->desc.data();
            compress_her_out->draw_into_cached_bitmap(dc);
          }
          compress_her_out->draw_compressor_pic_from_cached(dc);
          need_redraw_output_compressor=false;
        }
        compress_her_out->draw_LEDs_from_cached(dc);
      }
      if (compress_her_final){
        compress_her_final->x=(float)final_dummy_for_compressor->x;
        compress_her_final->y=(float)final_dummy_for_compressor->y;
        if (need_redraw_final_compressor){
          if (compress_her_final->needs_redraw){
            //              compress_her_final->desc=final_dummy_for_compressor->desc.data();
            compress_her_final->draw_into_cached_bitmap(dc);
          }
          compress_her_final->draw_compressor_pic_from_cached(dc);
          need_redraw_final_compressor=false;
        }
        compress_her_final->draw_LEDs_from_cached(dc);
      }
      final_meat->draw(hw,dc,output_gain_normalized);
      crest_factor_meat->draw(hw,dc);
      static int bung0=0;
      is_hidden=dummy_for_BASSMON->hidden&hide_extended_controls;
      if (bung0--<0 && !is_hidden){
        bung0=1;
        bm->draw(dc,graph_offset_in_dBs,rta_shows_peaks!=0);
      }

      static int bung=0;
      if (bung--<0){
        bung=1;
        if (max_output!=0){
          max_output_now=max_output;
          float max_output_now_dbs=10.0f*log10fRTT(fabsf(max_output_now));
#define MAX_OUTPUT_EMA_ALPHA (2.0f/100.0f)
          max_output_displayed=max_output_displayed*(1.0f-MAX_OUTPUT_EMA_ALPHA)+MAX_OUTPUT_EMA_ALPHA*max_output_now_dbs;
          max_output_displayed=max(max_output_displayed,max_output_now_dbs);
          //            ST2("MAX O %f\n",max_output_displayed);
          output_level_PUNCHED_UP_TO_THE_MAX_knob->set_control_to_this_normalized_float_value((max_output_displayed+30.0f)/60.0f);
          output_level_PUNCHED_UP_TO_THE_MAX_knob->plot(dc,hw);
          output_level_headroom_knob->set_control_to_this_normalized_float_value((dbs_of_big_knob_wants_to_be-max_output_displayed+30.0f)/60.0f);
          output_level_headroom_knob->plot(dc,hw);
          float mo_now_dbs=10.0f*log10fRTT(fabsf(max_output_now));
          output_level_instantaneous_knob->set_control_to_this_normalized_float_value((mo_now_dbs+30.0f)/60.0f);
          output_level_instantaneous_knob->plot(dc,hw);
          max_output=1e-10f;
        }else{
          max_output_now=0;
        }

        if (rms_num_samples>0){
#define RMS_OUTPUT_EMA_ALPHA (2.0f/100.0f)
          float rms_output_now=sqrtf(rms_squared_sum/(float)rms_num_samples);
          rms_output_smoothed=rms_output_smoothed*(1.0f-RMS_OUTPUT_EMA_ALPHA)+RMS_OUTPUT_EMA_ALPHA*rms_output_now;
          //          rms_output_smoothed=max(rms_output_smoothed,rms_output_now);
          float rms_output_smoothed_dbs=10.0f*log10fRTT(fabsf(rms_output_smoothed));
          output_level_RMS_knob->set_control_to_this_normalized_float_value((rms_output_smoothed_dbs+30.0f)/60.0f);
          output_level_RMS_knob->plot(dc,hw);
        }
        rms_squared_sum=0;
        rms_num_samples=0;
      }
      if (meat){
        meat->draw(hw,dc);
      }
//      if (player){
//        player->draw(dc,dummy_for_player_knob->x,dummy_for_player_knob->y);
//      }
//      if (excurse){
//        excurse->draw(dc,dummy_for_excursion_monitor_knob->x,dummy_for_excursion_monitor_knob->y);
//      }
      if (xwaz_console->console_needs_updaing){
        xwaz_console->draw(dc,hw,(float)fplayer->current_sample_track_time);
      }
      if (my_noco->console_needs_updaing){
        my_noco->draw(dc,hw);
      }
      ReleaseDC(hw,dc);
    }
    break;
  case TIMER_EVENT_PHASE_METER_UPDATE_DATA:
    if (meat){
      if (!pause_phase_meter_update){
        meat->update_buffer();
      }
    }
    break;
  case TIMER_EVENT_PHASE_METER_PAUSE:
    pause_phase_meter_update^=true;
    break;
  case TIMER_EVENT_PHASE_METER_SHO_INFO:
    meat->show_info=false;
//    KillTimer(hw,TIMER_EVENT_PHASE_METER_SHO_INFO);
    break;
  case TIMER_EVENT_SYNCHRONISED_FLASHING:
////    synchronised_flash_frame^=1;
    for(int i=0;i<(int)knobs.size();i++){
      control_pic *k=knobs[i];
      if ((k->make_me_attract_flash || k->is_flashing) && k->displayed_frame==1){
        k->Invalidate_me(hw);
      }
    }
    break;
//  case TIMER_EVENT_FPS:
//    renders_last_1_second=renders;
//    timer_render_updates_last_1_second=updates;
//    updates=0;
//    renders=0;
//    InvalidateRect(hw,&Crectal(0,0,30,8),true);
//    break;
//  case TIMER_EVENT_DO_A_BIG_REDRAW:
//    KillTimer(hw,TIMER_EVENT_DO_A_BIG_REDRAW);
//    InvalidateRect(hw,0,true);
//    redraw_unredraw_with_invalidate_things(hw);
//    break;
//  case TIMER_EVENT_CHECK_AUDIO_DECODING:
//    player->techno->check_buffers_and_trackchange();
//    //      player->track2->check_decoding();
//    break;
  }
}





void processing::load_ui_config2(int cfg){
  //  return;
  string filen="config2_NEW_"+lexical_cast<string>(cfg)+".txt";
  ifstream f(filen.data());
  string data;
  map<string,control_temp> control_load_lut;
//  int n;
//  getline(f,data);
//  sscanf_s(data.data(),"%d",&n);
  while(!f.eof()){
    string name_hash;
    getline(f,name_hash);
    control_temp t;
    getline(f,data);
    sscanf_s(data.data(),"%d",&t.x);
    getline(f,data);
    sscanf_s(data.data(),"%d",&t.y);
    control_load_lut[name_hash]=t;
  }
  f.close();
  for(int i=0;i<(int)knobs.size();i++){
    control_pic *k=knobs[i];
    string hash=string("processor_UI_")+lexical_cast<string>(processor_index)+"__"+k->group+"____"+k->desc;
    if (control_load_lut.find(hash)!=control_load_lut.end()){
      control_temp &cp=control_load_lut[hash];
      //      assert(k->x==cp.x &&k->y==cp.y);
      k->x=cp.x+plot_x_offset;
      k->y=cp.y+plot_y_offset;
    }else{
      string hash=k->group+"____"+k->desc;
      _cprintf("control %s not found in config tryin new one...\n",hash);
      if (control_load_lut.find(hash)!=control_load_lut.end()){
        control_temp &cp=control_load_lut[hash];
        //      assert(k->x==cp.x &&k->y==cp.y);
        k->x=cp.x+plot_x_offset;
        k->y=cp.y+plot_y_offset;
      }else{
        _cprintf("control %s not found in config...\n",hash);
      }
    }
  }
  RTAL_global_x=(float)dummy_for_RTAL->x;
  RTAL_global_y=(float)dummy_for_RTAL->y;
  RTAL_global_width=(float)dummy_for_RTAL->width;
  RTAL_global_height=(float)dummy_for_RTAL->height;
  RTAR_global_x=(float)dummy_for_RTAR->x;
  RTAR_global_y=(float)dummy_for_RTAR->y;
  RTAR_global_width=(float)dummy_for_RTAR->width;
  RTAR_global_height=(float)dummy_for_RTAR->height;
  BASSMON_global_x=(float)dummy_for_BASSMON->x;
  BASSMON_global_y=(float)dummy_for_BASSMON->y;
  BASSMON_global_width=(float)dummy_for_BASSMON->width;
  BASSMON_global_height=(float)dummy_for_BASSMON->height;
  COMPSSB_global_x=(float)bass_her_dummy_for_compressor->x;
  COMPSSB_global_y=(float)bass_her_dummy_for_compressor->y;
  COMPSSB_global_width=(float)bass_her_dummy_for_compressor->width;
  COMPSSB_global_height=(float)bass_her_dummy_for_compressor->height;
  OUTPUTBASS_global_x=(float)output_dummy_for_compressor->x;
  OUTPUTBASS_global_y=(float)output_dummy_for_compressor->y;
  OUTPUTBASS_global_width=(float)output_dummy_for_compressor->width;
  OUTPUTBASS_global_height=(float)output_dummy_for_compressor->height;
  bass_channelBASS_global_x=(float)bass_channel_dummy_for_compressor->x;
  bass_channelBASS_global_y=(float)bass_channel_dummy_for_compressor->y;
  bass_channelBASS_global_width=(float)bass_channel_dummy_for_compressor->width;
  bass_channelBASS_global_height=(float)bass_channel_dummy_for_compressor->height;
  compress_her_bass_LED_rect_global=Crectal((int)bass_channel_dummy_for_compressor_LEDs->x,(int)bass_channel_dummy_for_compressor_LEDs->y,(int)bass_channel_dummy_for_compressor_LEDs->x+(int)bass_channel_dummy_for_compressor_LEDs->width,(int)bass_channel_dummy_for_compressor_LEDs->y+(int)bass_channel_dummy_for_compressor_LEDs->height);
  compress_her_ssb_LED_rect_global=Crectal((int)bass_her_dummy_for_compressor_LEDs->x,(int)bass_her_dummy_for_compressor_LEDs->y,(int)bass_her_dummy_for_compressor_LEDs->x+(int)bass_her_dummy_for_compressor_LEDs->width,(int)bass_her_dummy_for_compressor_LEDs->y+(int)bass_her_dummy_for_compressor_LEDs->height);
  compress_her_out_LED_rect_global=Crectal((int)output_dummy_for_compressor_LEDs->x,(int)output_dummy_for_compressor_LEDs->y,(int)output_dummy_for_compressor_LEDs->x+(int)output_dummy_for_compressor_LEDs->width,(int)output_dummy_for_compressor_LEDs->y+(int)output_dummy_for_compressor_LEDs->height);

}








#define PREPEND_GROUP_TO_STRING(R) (global_channel_name_prefix+"____"+string(R)).data()


void processing::create_knobs(){
  vector<string> images;
  int xx=10;
  int yy=10;
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_3.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_4.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_5.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_6.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_7.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_8.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_9.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_10.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_11.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_12.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_13.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_14.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_15.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_16.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_17.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_18.bmp");
  images.push_back("knobs\\RTT_MEGA_KOK_2012_4_19.bmp");
  schnurtgren_guibertube_selector_knob=add_dial_multi_image(images,xx+80,yy+250,0,40.0f,320.0f,&global_shnurtgren,"RTTSG","selector, too hard for real",true);

  add_dial("knobs\\ultra_hard_and_widening_knob3_2.bmp",xx+300,yy+100,0,60.0f,300.0f,&global_bas_width,"multi-band widener","low width");
  add_dial("knobs\\ultra_hard_and_widening_knob3_2.bmp",xx+400,yy+100,0,60.0f,300.0f,&global_mid_width,"multi-band widener","mid width");
  add_dial("knobs\\ultra_hard_and_widening_knob3_2.bmp",xx+500,yy+100,0,60.0f,300.0f,&global_top_width,"multi-band widener","top width");

  add_dial("knobs\\bands_-1.00_to+2.00_step0.01_green_0.00.bmp",xx+300,yy+220,0,60.0f,300.0f,&global_bas_gain_wants_to_be,"equalizer","low gain");
  add_dial("knobs\\bands_-1.00_to+2.00_step0.01_green_0.00.bmp",xx+400,yy+220,0,60.0f,300.0f,&global_mid_gain_wants_to_be,"equalizer","mid gain");
  add_dial("knobs\\bands_-1.00_to+2.00_step0.01_green_0.00.bmp",xx+500,yy+220,0,60.0f,300.0f,&global_top_gain_wants_to_be,"equalizer","top gain");
  add_dial("knobs\\sharpening.bmp",xx+300,yy+330,0,60.0f,300.0f,&global_sharpen_ammount,"sharpener","ammount");
  dummy_for_RTAL              =new control_pic((int)RTAL_global_x,(int)RTAL_global_y,(int)RTAL_global_width,(int)RTAL_global_height,&graph_offset_in_dBs,PREPEND_GROUP_TO_STRING("RTA Left"),"main display",false);
  dummy_for_RTAR              =new control_pic((int)RTAR_global_x,(int)RTAR_global_y,(int)RTAR_global_width,(int)RTAR_global_height,&graph_offset_in_dBs,PREPEND_GROUP_TO_STRING("RTA Right"),"main display1",false);
  dummy_for_BASSMON           =new control_pic((int)BASSMON_global_x,(int)BASSMON_global_y,(int)BASSMON_global_width,(int)BASSMON_global_height,&graph_offset_in_dBs,PREPEND_GROUP_TO_STRING("RTA Right"),"main display2",false);
  knobs.push_back(dummy_for_RTAL);
  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&sharpen_the_nig,"sharpener","on or off",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&widen_the_nig,"multi-band widener","on or off",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&equalize_the_nig,"equalizer","on or off1",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
//  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&RTTSG_on_or_off,"RTTSG","on or off",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,true);
  control_pic *t=add_switch("knobs\\togal_58px2.bmp",xx+540,yy+400,&rta_shows_peaks,"RTA","peak hold",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,true);t->frames[0]->override_frame_height=44;
//  normalized_rate_to_track_parameter_changes_knob=add_dial("knobs\\paramchange_time_KOAB3.bmp",xx+450,yy+330,0,60.0f,300.0f,&normalized_rate_to_track_parameter_changes_nob_val,"RTTSG","parameter change rate",true);

//  images.clear();
//  images.push_back("knobs\\ABC_flat_curve3.bmp");
//  images.push_back("knobs\\ABC_flat_curve4.bmp");
//  images.push_back("knobs\\ABC_flat_curve5.bmp");
//  images.push_back("knobs\\ABC_flat_curve6.bmp");
//  add_dial_multi_image(images,xx+80,yy+250,0,152.0f,208.0f,&weighting_curve_float,"RTA","Perceptual weighting curve",true);

  add_dial("knobs\\ssb_bass_boost_brown.bmp",xx+900,yy+220,0,60.0f,300.0f,&bass_her_ammount_of_bass_boost_wants_to_be,"ssb generator","Output gain");
  add_dial("knobs\\30-90hz_2.bmp",xx+900,yy+520,0,60.0f,300.0f,&bass_her_clean_up_low_pass_index_normalized,"ssb generator","output lowpass");
  add_dial("knobs\\10-300hz_2.bmp",xx+500,yy+400,0,60.0f,300.0f,&bass_her_weaver_ssb_carrier_frequency_normalized,"ssb generator","Weaver SSB carrier frequency");
  add_dial("knobs\\10-300hz_2.bmp",xx+600,yy+430,0,60.0f,300.0f,&bass_her_weaver_ssb_bandwidth_normalized,"ssb generator","Weaver SSB bandwidth");
  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+700,&bass_her_weaver_ssb_onoff,"ssb generator","ssb on or off",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
  add_dial("knobs\\10-65hz_2.bmp",xx+600,yy+480,0,60.0f,300.0f,&bass_her_clean_up_highpass_index_normalized,"ssb generator","output highpass");
  bass_her_pass_only_extra_bass_onoff_knob=add_switch("knobs\\RTT_GREEN_RADIO_76px.bmp",xx+540,yy+490,&bass_her_pass_only_extra_bass_onoff,"ssb generator","output only extra signal",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);bass_her_pass_only_extra_bass_onoff_knob->make_me_attract_flash_when_selected();
  add_dial("knobs\\weaver_centre_LP.bmp",xx+600,yy+480,0,90.0f,180.0f,&weaver_ssb_centre_low_pass_filter_index_normalized,"ssb generator","centre lowpass");

  sharpener_light=add_light("knobs\\RTT_ORANGE_LIGHT_76px.bmp",xx+740,yy+100,"sharpener indicator","on or off");
  widener_light=add_light("knobs\\RTT_ORANGE_LIGHT_76px.bmp",xx+740,yy+150,"multi-band widener indicator","on or off");
  equalizer_light=add_light("knobs\\RTT_ORANGE_LIGHT_76px.bmp",xx+740,yy+200,"equalizer indicator","on or off");
  weaver_ssb_light=add_light("knobs\\RTT_ORANGE_LIGHT_76px.bmp",xx+740,yy+250,"ssb generator indicator","ssb on or off");

  add_switch("knobs\\RTT_GREEN_RADIO_38px.bmp",xx+340,yy+190,&auto_load_presets_on_off_radio,"RTTSG Selector","Auto load presets as selector moves",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,true);
  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+290,&save_preset_radio,"RTTSG Selector","save preset & overwrite current selector preset",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));
  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+390,&undo_radio,"RTTSG Selector","undo",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));
  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+490,&redo_radio,"RTTSG Selector","redo",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));

  rta_mode_select_normal_radio=add_switch("knobs\\RTT_BLUE_RADIO_32px.bmp",xx+340,yy+190,&rta_mode_select_dummy,"RTA Selector","instantaneous value",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
  rta_mode_select_text_radio=add_switch("knobs\\RTT_BLUE_RADIO_32px.bmp",xx+340,yy+210,&rta_mode_select_dummy,"RTA Selector","instantaneous text",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);
  rta_mode_select_text_above_radio=add_switch("knobs\\RTT_BLUE_RADIO_32px.bmp",xx+340,yy+230,&rta_mode_select_dummy,"RTA Selector","instantaneous value+text",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,false);

//  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+390,&UI_layout_preset_load,"UI layout","load",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));
//  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+490,&UI_layout_preset_save,"UI layout","save",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));
//  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+590,&UI_layout_preset_default,"UI layout","default",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));

//  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&RTTSG_bypass_on_or_off,"RTTSG","bypass on or off",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,true);
//  RTTSG_onoff_light=add_light("knobs\\RTT_WHITE_LIGHT_76px.bmp",xx+740,yy+350,"RTTSG","on off light");
//  RTTSG_bypass_light=add_light("knobs\\RTT_GREEN_LIGHT_76px.bmp",xx+740,yy+450,"RTTSG","bypass light");
  knobs.push_back(dummy_for_RTAR);
  knobs.push_back(dummy_for_BASSMON);
  bass_her_dummy_for_compressor           =new control_pic((int)COMPSSB_global_x,(int)COMPSSB_global_y,(int)COMPSSB_global_width,(int)COMPSSB_global_height,&dummydummy,PREPEND_GROUP_TO_STRING("ssb generator compressor"),SUB_BASS_COMPRESSOR_DESC_STRING,false);
  knobs.push_back(bass_her_dummy_for_compressor);

  bass_her_compressor_threshold_knob=add_dial("knobs\\compressor_threshold3.bmp",xx+1200,yy+220,0,30.0f,330.0f,&bass_her_compressor_threshold_normalized,"ssb generator compressor","Compressor threshold",RELOAD_FROM_PRESET);
  bass_her_compressor_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+240,0,30.0f,330.0f,&bass_her_compressor_softness_normalized,"ssb generator compressor","Compressor knee softness",RELOAD_FROM_PRESET);
  bass_her_compressor_ratio_knob=add_dial("knobs\\compressor_ratio72.bmp",xx+1200,yy+260,0,30.0f,330.0f,&bass_her_compressor_ratio_normalized,"ssb generator compressor","Compressor ratio",RELOAD_FROM_PRESET);
  bass_her_limiter_threshold_knob=add_dial("knobs\\limiter_threshold3.bmp",xx+1200,yy+280,0,30.0f,330.0f,&bass_her_limiter_threshold_normalized,"ssb generator compressor","Limiter threshold",RELOAD_FROM_PRESET);
  bass_her_limiter_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+300,0,30.0f,330.0f,&bass_her_limiter_softness_normalized,"ssb generator compressor","Limiter knee softness",RELOAD_FROM_PRESET);
  bass_her_compressor_attack_knob=add_dial("knobs\\compressor_attack.bmp",xx+1250,yy+480,0,30.0f,330.0f,&bass_her_compressor_attack_normalized,"ssb generator compressor","compressor attack",RELOAD_FROM_PRESET);
  bass_her_compressor_release_knob=add_dial("knobs\\compressor_release.bmp",xx+1250,yy+580,0,30.0f,330.0f,&bass_her_compressor_release_normalized,"ssb generator compressor","compressor release",RELOAD_FROM_PRESET);
  bass_her_dummy_for_compressor_LEDs      =new control_pic(800,300,COMPRESSOR_LED_DISPLAY_WIDTH,COMPRESSOR_LED_DISPLAY_HEIGHT,&bass_her_dummy_for_compressor_LEDS_normalized,PREPEND_GROUP_TO_STRING("ssb generator compressor"),"LED Display",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(bass_her_dummy_for_compressor_LEDs);

  big_one=add_dial("knobs\\clips_at_27_2.bmp",xx+1250,yy+580,0,60.0f,300.0f,&output_gain_normalized,"Output","output scaling",DONT_RELOAD_FROM_PRESET);
  output_level_PUNCHED_UP_TO_THE_MAX_knob=add_dial("knobs\\output_level_RED_0.1db.bmp",xx+1250,yy+580,0,60.0f,300.0f,&output_level_PUNCHED_UP_TO_THE_MAX_normalized,"output","current output level PUTTM",DONT_RELOAD_FROM_PRESET);
  output_level_PUNCHED_UP_TO_THE_MAX_knob->set_override_frame_height(30);

  output_compressor_threshold_knob=add_dial("knobs\\compressor_threshold3.bmp",xx+1200,yy+220,0,30.0f,330.0f,&output_compressor_threshold_normalized,"output compressor","Compressor threshold",RELOAD_FROM_PRESET);
  output_compressor_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+340,0,30.0f,330.0f,&output_compressor_softness_normalized,"output compressor","Compressor knee softness",RELOAD_FROM_PRESET);
  output_compressor_ratio_knob=add_dial("knobs\\compressor_ratio72.bmp",xx+1200,yy+460,0,30.0f,330.0f,&output_compressor_ratio_normalized,"output compressor","Compressor ratio",RELOAD_FROM_PRESET);
  output_limiter_threshold_knob=add_dial("knobs\\limiter_threshold3.bmp",xx+1200,yy+580,0,30.0f,330.0f,&output_limiter_threshold_normalized,"output compressor","Limiter threshold",RELOAD_FROM_PRESET);
  output_limiter_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+300,0,30.0f,330.0f,&output_limiter_softness_normalized,"output compressor","Limiter knee softness",RELOAD_FROM_PRESET);
  output_compressor_attack_knob=add_dial("knobs\\compressor_attack.bmp",xx+1250,yy+480,0,30.0f,330.0f,&output_compressor_attack_normalized,"output compressor","compressor attack",RELOAD_FROM_PRESET);
  output_compressor_release_knob=add_dial("knobs\\compressor_release.bmp",xx+1250,yy+580,0,30.0f,330.0f,&output_compressor_release_normalized,"output compressor","compressor release",RELOAD_FROM_PRESET);
  output_dummy_for_compressor_LEDs      =new control_pic(800,300,COMPRESSOR_LED_DISPLAY_WIDTH,COMPRESSOR_LED_DISPLAY_HEIGHT,&bass_her_dummy_for_compressor_LEDS_normalized,PREPEND_GROUP_TO_STRING("output compressor"),"LED Display",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(output_dummy_for_compressor_LEDs);
  output_dummy_for_compressor           =new control_pic((int)OUTPUTBASS_global_x-200,(int)OUTPUTBASS_global_y,(int)OUTPUTBASS_global_width,(int)OUTPUTBASS_global_height,&dummydummy,PREPEND_GROUP_TO_STRING("output compressor"),MID_HIGH_COMPRESSOR_DESC_STRING,false);
  knobs.push_back(output_dummy_for_compressor);

  bass_channel_compressor_threshold_knob=add_dial("knobs\\compressor_threshold3.bmp",xx+1200,yy+220,0,30.0f,330.0f,&bass_channel_compressor_threshold_normalized,"bass channel compressor","Compressor threshold",RELOAD_FROM_PRESET);
  bass_channel_compressor_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+240,0,30.0f,330.0f,&bass_channel_compressor_softness_normalized,"bass channel compressor","Compressor knee softness",RELOAD_FROM_PRESET);
  bass_channel_compressor_ratio_knob=add_dial("knobs\\compressor_ratio72.bmp",xx+1200,yy+260,0,30.0f,330.0f,&bass_channel_compressor_ratio_normalized,"bass channel compressor","Compressor rati3",RELOAD_FROM_PRESET);
  bass_channel_limiter_threshold_knob=add_dial("knobs\\limiter_threshold3.bmp",xx+1200,yy+280,0,30.0f,330.0f,&bass_channel_limiter_threshold_normalized,"bass channel compressor","Limiter threshold",RELOAD_FROM_PRESET);
  bass_channel_limiter_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+300,0,30.0f,330.0f,&bass_channel_limiter_softness_normalized,"bass channel compressor","Limiter knee softnes3",RELOAD_FROM_PRESET);
  bass_channel_compressor_attack_knob=add_dial("knobs\\compressor_attack.bmp",xx+1250,yy+480,0,30.0f,330.0f,&bass_channel_compressor_attack_normalized,"bass channel compressor","compressor attack",RELOAD_FROM_PRESET);
  bass_channel_compressor_release_knob=add_dial("knobs\\compressor_release.bmp",xx+1250,yy+580,0,30.0f,330.0f,&bass_channel_compressor_release_normalized,"bass channel compressor","compressor release",RELOAD_FROM_PRESET);
  bass_channel_dummy_for_compressor_LEDs      =new control_pic(800,300,COMPRESSOR_LED_DISPLAY_WIDTH,COMPRESSOR_LED_DISPLAY_HEIGHT,&bass_channel_dummy_for_compressor_LEDS_normalized,PREPEND_GROUP_TO_STRING("bass channel compressor"),"LED Display",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(bass_channel_dummy_for_compressor_LEDs);
  bass_channel_dummy_for_compressor           =new control_pic(200,200,(int)bass_channelBASS_global_width,(int)bass_channelBASS_global_height,&dummydummy,PREPEND_GROUP_TO_STRING("bass channel compressor"),BASS_COMPRESSOR_DESC_STRING,DONT_RELOAD_FROM_PRESET);
  knobs.push_back(bass_channel_dummy_for_compressor);

  bass_her_compressor_bypass_knob=add_switch("knobs\\compressor_bypass5.bmp",xx+640,yy+400,&bass_her_compressor_bypass,"ssb compressor","bypass ssb compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  bass_her_compressor_unique_knob=add_switch("knobs\\compressor_unique5.bmp",xx+640,yy+470,&bass_her_compressor_unique,"ssb compressor","unique output",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  output_compressor_bypass_knob=add_switch("knobs\\compressor_bypass5.bmp",xx+640,yy+480,&output_compressor_bypass,"output compressor","bypass output compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  output_compressor_unique_knob=add_switch("knobs\\compressor_unique5.bmp",xx+640,yy+490,&output_compressor_unique,"output compressor","unique output",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  bass_channel_compressor_bypass_knob=add_switch("knobs\\compressor_bypass5.bmp",xx+640,yy+500,&bass_channel_compressor_bypass,"bass channel compressor","bypass bass compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  bass_channel_compressor_unique_knob=add_switch("knobs\\compressor_unique5.bmp",xx+640,yy+510,&bass_channel_compressor_unique,"bass channel compressor","unique output",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);

  bass_her_compressor_unique_knob->radio_group=RADIO_GROUP_ROUTE_UNIQUELY_TO_OUTPUT;
  output_compressor_unique_knob->radio_group=RADIO_GROUP_ROUTE_UNIQUELY_TO_OUTPUT;
  bass_channel_compressor_unique_knob->radio_group=RADIO_GROUP_ROUTE_UNIQUELY_TO_OUTPUT;

  bass_her_compressor_unique_knob_global=     bass_her_compressor_unique_knob;
  output_compressor_unique_knob_global=       output_compressor_unique_knob;
  bass_channel_compressor_unique_knob_global= bass_channel_compressor_unique_knob;

  bass_her_compressor_bypass_knob_global=bass_her_compressor_bypass_knob;
  output_compressor_bypass_knob_global=output_compressor_bypass_knob;
  bass_channel_compressor_bypass_knob_global=bass_channel_compressor_bypass_knob;

  bass_her_compressor_lock_knob=add_switch("knobs\\compressor_lock5.bmp",xx+640,yy+400,&bass_her_compressor_lock,"ssb compressor","lock ssb compressor",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  output_compressor_lock_knob=add_switch("knobs\\compressor_lock5.bmp",xx+640,yy+480,&output_compressor_lock,"output compressor","lock output compressor",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  bass_channel_compressor_lock_knob=add_switch("knobs\\compressor_lock5.bmp",xx+640,yy+500,&bass_channel_compressor_lock,"bass compressor","lock bass compressor",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);

  output_compressor_output_knob=add_dial("knobs\\compressor_output.bmp",xx+500,yy+540,0,30.0f,330.0f,&output_compressor_output_normalized,"output compressor","compressor output",RELOAD_FROM_PRESET);
  bass_channel_compressor_output_knob=add_dial("knobs\\compressor_output.bmp",xx+500,yy+560,0,30.0f,330.0f,&bass_channel_compressor_output_normalized,"bass channel compressor","compressor output",RELOAD_FROM_PRESET);
  bass_her_compressor_output_knob=add_dial("knobs\\compressor_output.bmp",xx+500,yy+580,0,30.0f,330.0f,&bass_her_compressor_output_normalized,"ssb generator","compressor output",RELOAD_FROM_PRESET);

  dummy_for_face_meat_her_knob           =new control_pic(200,200,340,398,&dummydummy,PREPEND_GROUP_TO_STRING("Phase meter"),"LR phase",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(dummy_for_face_meat_her_knob);

  add_dial("knobs\\phase_meat.bmp",xx+500,yy+540,0,60.0f,310.0f,&phase_meater_zoom_rate_normalized,"phase meter","zoom tracking rate",DONT_RELOAD_FROM_PRESET);
  add_dial("knobs\\phase_meat.bmp",xx+500,yy+560,0,60.0f,310.0f,&phase_meater_x_zoom,"phase meter","aspect ratio separation-zoom",DONT_RELOAD_FROM_PRESET);
  add_dial("knobs\\phase_meat.bmp",xx+500,yy+580,0,60.0f,310.0f,&phase_meater_update_rate,"phase meter","update rate",DONT_RELOAD_FROM_PRESET);
  add_dial("knobs\\phase_meat.bmp",xx+500,yy+580,0,60.0f,310.0f,&phase_meater_master_zoom,"phase meter","master zoom",DONT_RELOAD_FROM_PRESET);

  final_compressor_threshold_knob=add_dial("knobs\\compressor_threshold3.bmp",xx+1200,yy+220,0,30.0f,330.0f,&final_compressor_threshold_normalized,"final compressor","Compressor threshold",RELOAD_FROM_PRESET);
  final_compressor_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+240,0,30.0f,330.0f,&final_compressor_softness_normalized,"final compressor","Compressor knee softness",RELOAD_FROM_PRESET);
  final_compressor_ratio_knob=add_dial("knobs\\compressor_ratio72.bmp",xx+1200,yy+260,0,30.0f,330.0f,&final_compressor_ratio_normalized,"final compressor","Compressor ratio",RELOAD_FROM_PRESET);
  final_limiter_threshold_knob=add_dial("knobs\\limiter_threshold3.bmp",xx+1200,yy+280,0,30.0f,330.0f,&final_limiter_threshold_normalized,"final compressor","Limiter threshold",RELOAD_FROM_PRESET);
  final_limiter_softness_knob=add_dial("knobs\\compressor_sharpness72.bmp",xx+1200,yy+300,0,30.0f,330.0f,&final_limiter_softness_normalized,"final compressor","Limiter knee softness",RELOAD_FROM_PRESET);
  final_compressor_attack_knob=add_dial("knobs\\compressor_attack.bmp",xx+1250,yy+480,0,30.0f,330.0f,&final_compressor_attack_normalized,"final compressor","compressor attack",RELOAD_FROM_PRESET);
  final_compressor_release_knob=add_dial("knobs\\compressor_release.bmp",xx+1250,yy+580,0,30.0f,330.0f,&final_compressor_release_normalized,"final compressor","compressor release",RELOAD_FROM_PRESET);
  final_dummy_for_compressor_LEDs      =new control_pic(800,300,COMPRESSOR_LED_DISPLAY_WIDTH,COMPRESSOR_LED_DISPLAY_HEIGHT,&final_dummy_for_compressor_LEDS_normalized,PREPEND_GROUP_TO_STRING("final compressor"),"LED Display",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(final_dummy_for_compressor_LEDs);
  final_dummy_for_compressor           =new control_pic(200,200,(int)finalBASS_global_width,(int)finalBASS_global_height,&dummydummy,PREPEND_GROUP_TO_STRING("final compressor"),FINAL_MIX_COMPRESSOR_DESC_STRING,DONT_RELOAD_FROM_PRESET);
  knobs.push_back(final_dummy_for_compressor);
  final_compressor_bypass_knob=add_switch("knobs\\compressor_bypass5.bmp",xx+640,yy+500,&final_compressor_bypass,"final compressor","bypass bass compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);final_compressor_bypass_knob->frames[0]->override_frame_height=24;
  final_compressor_unique_knob=add_switch("knobs\\compressor_unique5.bmp",xx+640,yy+510,&final_compressor_unique,"final compressor","unique output",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);final_compressor_unique_knob->frames[0]->override_frame_height=24;
  final_compressor_bypass_knob_global=final_compressor_bypass_knob;
  final_compressor_lock_knob=add_switch("knobs\\compressor_lock5.bmp",xx+640,yy+500,&final_compressor_lock,"final compressor","lock bass compressor",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);final_compressor_lock_knob->frames[0]->override_frame_height=24;
  final_compressor_output_knob=add_dial("knobs\\compressor_output.bmp",xx+500,yy+560,0,30.0f,330.0f,&final_compressor_output_normalized,"final compressor","compressor output",RELOAD_FROM_PRESET);
  final_compressor_unique_knob->radio_group=RADIO_GROUP_ROUTE_UNIQUELY_TO_OUTPUT;

  dummy_for_output_meter_knob       =new control_pic(200,200,FINAL_METER_WIDTH,FINAL_METER_HEIGHT,&dummydummy,PREPEND_GROUP_TO_STRING("Output meter"),"LR and peak",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(dummy_for_output_meter_knob);


  bass_her_compressor_mute_knob=add_switch("knobs\\compressor_mute5.bmp",640,10,&ssb_compressor_mute,"ssb compressor","mute ssb compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  output_compressor_mute_knob=add_switch("knobs\\compressor_mute5.bmp",640,20,&output_compressor_mute,"output compressor","mute output compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  bass_channel_compressor_mute_knob=add_switch("knobs\\compressor_mute5.bmp",640,40,&bass_compressor_mute,"bass channel compressor","mute bass compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);
  final_compressor_mute_knob=add_switch("knobs\\compressor_mute5.bmp",640,50,&final_compressor_mute,"final compressor","mute bass compressor",IS_TOGGLE_SWITCH,IS_FLASHING_SWITCH,DONT_RELOAD_FROM_PRESET);final_compressor_mute_knob->frames[0]->override_frame_height=24;

  dummy_for_crest_factor_meter_knob           =new control_pic(200,200,CREST_FACTOR_METER_WIDTH,CREST_FACTOR_METER_HEIGHT,&dummydummy,PREPEND_GROUP_TO_STRING("Crest factor meter"),"Crest & headroom",DONT_RELOAD_FROM_PRESET);knobs.push_back(dummy_for_crest_factor_meter_knob);

  output_level_instantaneous_knob=add_dial("knobs\\output_level_GREEN_0.1db.bmp",xx+1250,yy+580,0,60.0f,300.0f,&output_level_instantaneous_normalized,"output","current output level instantaneous",DONT_RELOAD_FROM_PRESET);
  output_level_instantaneous_knob->set_override_frame_height(30);
  output_level_RMS_knob=add_dial("knobs\\output_level_ORANGE_0.1db.bmp",xx+1250,yy+590,0,60.0f,300.0f,&output_level_RMS_normalized,"output","current output level RMS",DONT_RELOAD_FROM_PRESET);
  output_level_RMS_knob->set_override_frame_height(30);
  output_level_headroom_knob=add_dial("knobs\\output_level_BLUE_0.1db.bmp",xx+1250,yy+600,0,60.0f,300.0f,&output_level_headroom_normalized,"output","current output level headroom",DONT_RELOAD_FROM_PRESET);
  output_level_headroom_knob->set_override_frame_height(30);
#define OUTPUT_LED_DISPLAY_METER_WIDTH 88
#define OUTPUT_LED_DISPLAY_METER_HEIGHT CREST_FACTOR_METER_HEIGHT
  dummy_for_output_led_output_meters_knob           =new control_pic(200,200,OUTPUT_LED_DISPLAY_METER_WIDTH,OUTPUT_LED_DISPLAY_METER_HEIGHT,&dummydummy,PREPEND_GROUP_TO_STRING("Info meters"),"...",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(dummy_for_output_led_output_meters_knob);

  add_dial("knobs\\equi_AH_koab.bmp",xx+1400,yy+220,0,30.0f,300.0f,&equalizer_bass_normalized,"equalizer","bass",RELOAD_FROM_PRESET);
  add_dial("knobs\\equi_AH_koab.bmp",xx+1400,yy+270,0,30.0f,300.0f,&equalizer_mid1_normalized,"equalizer","mid1",RELOAD_FROM_PRESET);
  add_dial("knobs\\equi_AH_koab.bmp",xx+1400,yy+320,0,30.0f,300.0f,&equalizer_mid2_normalized,"equalizer","mid2",RELOAD_FROM_PRESET);
  add_dial("knobs\\equi_AH_koab.bmp",xx+1400,yy+370,0,30.0f,300.0f,&equalizer_high_normalized,"equalizer","high",RELOAD_FROM_PRESET);
  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&equalizer_AH_on_or_off,"equalizer","on or off 2",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,RELOAD_FROM_PRESET);
  equalizer_AH_light=add_light("knobs\\RTT_ORANGE_LIGHT_76px.bmp",1440,300,"equalizer AH indicator","on or off");

  add_dial("knobs\\input_gain_1_2.bmp",1350,280,0,60.0f,300.0f,&input_gain_normalized,"Input","Input Gain",DONT_RELOAD_FROM_PRESET);

  schroeder_comb_delay1_knob=add_dial("knobs\\schroeder_comb_delay.bmp",xx+1400,50,0,40.0f,320.0f,&schroeder_comb_delay1_normalized,"RTT-Schroeder","comb filter 1 delay",RELOAD_FROM_PRESET);
  schroeder_comb_delay2_knob=add_dial("knobs\\schroeder_comb_delay.bmp",xx+1400,150,0,40.0f,320.0f,&schroeder_comb_delay2_normalized,"RTT-Schroeder","comb filter 2 delay",RELOAD_FROM_PRESET);
  schroeder_comb_delay3_knob=add_dial("knobs\\schroeder_comb_delay.bmp",xx+1400,250,0,40.0f,320.0f,&schroeder_comb_delay3_normalized,"RTT-Schroeder","comb filter 3 delay",RELOAD_FROM_PRESET);
  schroeder_comb_delay4_knob=add_dial("knobs\\schroeder_comb_delay.bmp",xx+1400,350,0,40.0f,320.0f,&schroeder_comb_delay4_normalized,"RTT-Schroeder","comb filter 4 delay",RELOAD_FROM_PRESET);
  schroeder_allpass_delay1_knob=add_dial("knobs\\schroeder_allpass_delay.bmp",xx+1500,50,0,40.0f,320.0f,&schroeder_allpass_delay1_normalized,"RTT-Schroeder","allpass filter 1 delay",RELOAD_FROM_PRESET);
  schroeder_allpass_delay2_knob=add_dial("knobs\\schroeder_allpass_delay.bmp",xx+1500,150,0,40.0f,320.0f,&schroeder_allpass_delay2_normalized,"RTT-Schroeder","allpass filter 2 delay",RELOAD_FROM_PRESET);
  schroeder_allpass_reverb1_knob=add_dial("knobs\\schroeder_allpass_reverb.bmp",xx+1500,250,0,40.0f,320.0f,&schroeder_allpass_reverb1_normalized,"RTT-Schroeder","allpass filter 1 reverb",RELOAD_FROM_PRESET);
  schroeder_allpass_reverb2_knob=add_dial("knobs\\schroeder_allpass_reverb.bmp",xx+1500,350,0,40.0f,320.0f,&schroeder_allpass_reverb2_normalized,"RTT-Schroeder","allpass filter 2 reverb",RELOAD_FROM_PRESET);
  schroeder_reverberation_time_knob=add_dial("knobs\\schroeder_reverberation_time2.bmp",xx+1500,350,0,40.0f,320.0f,&schroeder_reverberation_time_normalized,"RTT-Schroeder","reverberation time",RELOAD_FROM_PRESET);
  schroeder_mix_knob=add_dial("knobs\\schroeder_mix.bmp",xx+1500,450,0,60.0f,300.0f,&schroeder_mix_normalized,"schroeder","mid high reverb mix",RELOAD_FROM_PRESET);

  add_switch("knobs\\togal_BUNGCHONG.bmp",xx+540,yy+400,&schroeder_on_off,"RTT-Schroeder","on or off",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,RELOAD_FROM_PRESET);
  schroeder_on_off_light=add_light("knobs\\RTT_ORANGE_LIGHT_76px.bmp",xx+740,yy+350,"RTT-Schroeder","on off light");
  schroeder_low_pass_knob=add_dial("knobs\\schroeder_low_pass.bmp",xx+1500,330,0,30.0f,300.0f,&schroeder_comb_low_pass_normalized,"RTT-Schroeder","comb filter low pass freq",RELOAD_FROM_PRESET);

//  techno_knob=add_dial("knobs\\techno.bmp",xx+1500,330,0,60.0f,180.0f,&is_it_techno_normalized,"Selecta","get some good input",DONT_RELOAD_FROM_PRESET);

//  dummy_for_player_knob           =new control_pic(200,200,PLAYER_DISPLAY_WIDTH,PLAYER_DISPLAY_HEIGHT,&dummydummy,"Player","...",DONT_RELOAD_FROM_PRESET);
//  knobs.push_back(dummy_for_player_knob);
//
//  player_seek_knob=add_dial("knobs\\player_koorb1.bmp",xx+1500,350,0,1.0f,359.0f,&player_seek_normalized,"player","track seek",DONT_RELOAD_FROM_PRESET);player_seek_knob->frames_cycle=true;player_seek_knob->i_am_a_knob_with_a_momentary_switch_too=true;
//  player_track_select_knob=add_dial("knobs\\player_koorb2.bmp",xx+1500,350,0,1.0f,359.0f,&player_track_select_normalized,"player","track select",DONT_RELOAD_FROM_PRESET);player_track_select_knob->frames_cycle=true;player_track_select_knob->i_am_a_knob_with_a_momentary_switch_too=true;

  momentary_switches.push_back(add_switch("knobs\\radio_momentary.bmp",xx+340,yy+290,&mute_for_reverberator_testing,"RTT-Schroeder","Mute",IS_MOMENTARY_SWITCH,IS_NONFLASHING_SWITCH,true));

  schroeder_preset_knob=add_dial("knobs\\schroeder_preset.bmp",xx+1500,350,0,40.0f,320.0f,&schroeder_preset_normalized,"RTT-Schroeder","Load preset",DONT_RELOAD_FROM_PRESET);

  control_pic *temp=add_switch("knobs\\toggull_64px.bmp",1540,400,&equal_ricer_ellipic9_or_LR4,"Equalizer","ellipic9O or Linkwitz-Riley4O",IS_TOGGLE_SWITCH,IS_NONFLASHING_SWITCH,RELOAD_FROM_PRESET);temp->frames[0]->override_frame_height=47;
  schroeder_combs0_7_buffer_length_knob=add_dial("knobs\\schroeder_combs0-7_buffer_length.bmp",1500,150,0,40.0f,320.0f,&schroeder_combs0_7_buffer_length_normalized,"RTT-Schroeder","comb filters 0-7 buffer length",RELOAD_FROM_PRESET);

  control_pic::set_frame_clip_borders("knobs\\togal_BUNGCHONG.bmp",10,4);
  control_pic::set_frame_clip_borders("knobs\\RTT_ORANGE_LIGHT_76px.bmp",5,5);

  dummy_for_console_knob           =new control_pic(200,850,INDIVIDUAL_DECK_CONSOLE_WIDTH,INDIVIDUAL_DECK_CONSOLE_HEIGHT,&dummydummy,"Console","console 2",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(dummy_for_console_knob);

  dummy_for_xwaz_display_knob      =new control_pic(200,850,INDIVIDUAL_DECK_XWAZ_CONSOLE_WIDTH,INDIVIDUAL_DECK_XWAZ_CONSOLE_HEIGHT,&dummydummy,"xwaz","display",DONT_RELOAD_FROM_PRESET);
  knobs.push_back(dummy_for_xwaz_display_knob);

  load_ui_config2(0);
  set_schroeder_reverb_UI_knobz();
}

