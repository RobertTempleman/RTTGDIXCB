#include "OS_interface.h"
#include "compressher_limither.h"
#include "GBAtext.h"

bool need_to_recalc_params_and_use_atomics=false;
float max_output=0;
bool synchronised_flash_frame=true;



void compress_her_limit_her::SelectObject(RTTXCB* dc,HPEN &pen){
  select_dc_col=pen.col;
}


HPEN compress_her_limit_her::CreatePen(s32 style,s32 width,COLORREF col){
  return HPEN(style,width,col);
}


void compress_her_limit_her::SetDCPenColor2(RTTXCB* dc,COLORREF col){
  select_dc_col=col;
}


void compress_her_limit_her::SetDCBrushColor2(RTTXCB* dc,COLORREF col){
  select_dc_brush_col=col;
}


void compress_her_limit_her::Rectangle(RTTXCB* dc,s32 left,s32 top,s32 right,s32 bottom){
  dc->rectangle_fill(left, top, right-left, bottom-top, select_dc_col, select_dc_brush_col);
}



float log10fRTT(float a){
  if (a==0){
    return -200.0f;
  }
  return log10f(a);
}

float compress_her_limit_her::attack_time[NUM_COMPRESSOR_ATTACK_TIMES]={0.2f,0.3f,0.4f,0.5f,0.6f,0.8f,1.0f,1.2f,1.4f,1.7f,2.0f,2.5f,3.0f,3.5f,4.0f,5.0f,6.0f,7.0f,8.0f,10.0f,12.0f,15.0f,18.0f,21.0f,25.0f,30.0f,40.0f,50.0f,60.0f,70.0f,80.0f,90.0f,100.0f,125.0f,150.0f,175.0f,200.0f,300.0f,400.0f,500.0f,800.0f};
float compress_her_limit_her::release_time[NUM_COMPRESSOR_RELEASE_TIMES]={0.2f,0.3f,0.4f,0.5f,0.6f,0.8f,1.0f,1.2f,1.4f,1.7f,2.0f,2.5f,3.0f,3.5f,4.0f,5.0f,6.0f,7.0f,8.0f,10.0f,12.0f,15.0f,18.0f,21.0f,25.0f,30.0f,40.0f,50.0f,60.0f,70.0f,80.0f,90.0f,100.0f,125.0f,150.0f,175.0f,200.0f,300.0f,400.0f,500.0f,800.0f};
float compress_her_limit_her::ratios[NUM_COMPRESSOR_RATIOS]={1.0f,1.1f,1.2f,1.3f,1.4f,1.6f,1.8f,2.0f,2.5f,3.0f,4.0f,5.0f,6.0f,7.0f,8.0f,10.0f,12.0f,15.0f,18.0f,21.0f,25.0f};
float compress_her_limit_her::compressor_knee_radius[NUM_COMPRESSOR_KNEE_RADII]={00.05f,01.0f,02.0f,04.0f,06.0f,08.0f,10.0f,15.0f,20.0f,30.0f,50.0f};


float unlogit(float v){
  return powf(10.0f,v*0.1f);
}

float relogit(float v){
  return 10.0f*log10fRTT(v);
}



void soft_compressor::recalc_params(){
  end_of_softknee=start_of_softknee+start_of_softknee*0.5f;
  knee_circle_origin_x=start_of_softknee+knee_radius/sqrtf(2.0f);
  knee_circle_origin_y=start_of_softknee-knee_radius/sqrtf(2.0f);
  end_of_softknee=knee_circle_origin_x-knee_radius/sqrtf(1.0f+ratio*ratio);
  y_at_end_of_softknee=circle_y_as_fn_of_x(end_of_softknee);
}



soft_compressor::soft_compressor(float _compressor_trigger_level,float _ratio, float _knee_radius){
  knee_radius=_knee_radius;
  ratio=_ratio;
  start_of_softknee=_compressor_trigger_level;
  recalc_params();
}



float soft_compressor::circle_y_as_fn_of_x(float x){
  x=knee_circle_origin_x-x;
  return sqrtf(knee_radius*knee_radius-x*x)+knee_circle_origin_y;
}

float isqrt2=1.0f/sqrtf(2.0f);

float soft_compressor::doit(float in){
  float out=in;
  compressing=false;
  on_knee=false;
  float circ_x=-knee_radius*isqrt2;
  float circ_y=knee_radius-knee_radius*isqrt2;
  bool draw_ellipse=false;
  if (in>=end_of_softknee){
    out=y_at_end_of_softknee+(in-end_of_softknee)/ratio;
    compressing=true;
  }else if (in>start_of_softknee){
    out=circle_y_as_fn_of_x(in);
    on_knee=true;
  }
  return out;
}



void soft_limiter::recalc_params(){
  knee_circle_origin_y=limit-knee_radius;
  knee_circle_origin_x=limit+knee_radius*(sqrtf(2.0f)-1);

  start_of_softknee=limit-knee_radius+knee_radius/sqrtf(2.0f);
  end_of_softknee=knee_circle_origin_x;
}

soft_limiter::soft_limiter(float _limiter_max_val,float _knee_radius){
  avoid_redrawing_me_yet=false;
  knee_radius=_knee_radius;
  limit=_limiter_max_val;
  recalc_params();
}

float soft_limiter::circle_y_as_fn_of_x(float x){
  x=knee_circle_origin_x-x;
  return sqrtf(knee_radius*knee_radius-x*x)+knee_circle_origin_y;
}


float dick_tube_start_of_softknee;
float dick_tube_knee_radius;
float dick_tube_end_of_softknee;

float soft_limiter::doit(float in){
  dick_tube_start_of_softknee=start_of_softknee;
  dick_tube_knee_radius=knee_radius;
  dick_tube_end_of_softknee=end_of_softknee;

  float out=in;
  compressing=false;
  on_knee=false;
  float circ_x=-knee_radius*isqrt2;
  float circ_y=knee_radius-knee_radius*isqrt2;
  bool draw_ellipse=false;
  if (in>=end_of_softknee){
    out=limit;
    compressing=true;
  }else if (in>start_of_softknee){
    out=circle_y_as_fn_of_x(in);
    on_knee=true;
  }
  return out;
}



void compress_her_limit_her::set_params(float _compressor_trigger_level_dbs,float _ratio, float _knee_sharpness_dbs,float _limiter_level_dbs,float _limiter_sharpness_dbs){
  compressor=new soft_compressor(_compressor_trigger_level_dbs,_ratio,_knee_sharpness_dbs);
  limiter=new soft_limiter(_limiter_level_dbs,_limiter_sharpness_dbs);
  limiter_snapshot=limiter;
}


#define  PEN_THINKNOSS 1
compress_her_limit_her::compress_her_limit_her(RTTXCB &_rttxcb,
                                               const char *_desc,
                                               float _x,
                                               float _y,
                                               float _graph_width,
                                               float _graph_height,
                                               RECT&_LED_rect):rttxcb(_rttxcb),
                                                               x(_x),
                                                               y(_y),
                                                               width(_graph_width),
                                                               height(_graph_height),
                                                               LED_rect(_LED_rect){
  min_current_dynamics_reduction=1.0f;
  min_current_limiter_reduction=1.0f;
  ready_to_draw=false;
  bypass=false;
  gain=1.0f;
  current_dynamics_gain=1.0f;
  current_limiter_gain=1.0f;
  attack=10.0f;
  release=100.0f;
  max_val=0;
  rms_counter=0;
  gain_tracking_rate=0;

  //  buttmap=0;
  //  bitmap_dc=0;
  //  buttmap_LED=0;
  //  bitmap_LED_dc=0;
  desc=_desc;
//  desc="dum-e";

  needs_redraw=true;
//  cached_display=new Bitmap(width,height,PixelFormat24bppRGB);
//  graph_start_dB=-12.0f;
//  graph_end_dB=10.0f;
  graph_start_dB=-40.0f;
  graph_end_dB=20.0f;
  set_params(-3.0f,2.0f,10.0f,5.0f,2.0f);
//  penDot=CreatePen(PS_DOT,1,darkslateblue_c);
  penDot=CreatePen(PS_DOT,1,grey25_c);
  penDotzero_db_in=CreatePen(PS_DOT,1,grey25_c);
  Pen2green_pen=CreatePen(PS_SOLID,PEN_THINKNOSS,springgreen3_c);
  Pen2yellow_pen=CreatePen(PS_SOLID,PEN_THINKNOSS,orange_c);
  Pen2orange_pen=CreatePen(PS_SOLID,PEN_THINKNOSS,royalblue1_c);
  Pen2red_pen=CreatePen(PS_SOLID,PEN_THINKNOSS,yellow2_c);
  Pen2white_pen=CreatePen(PS_SOLID,PEN_THINKNOSS,tomato1_c);
  limiter_hold_ON_countdown=0;
  limiter_hold_gain=1.0f;
  limiter_hold_activation_counter=0;
  changed_parameters_so_release_limiter_counter=0;
  reset_minimum_limiter_gain_for_last_second_counter=44100;
  minimum_limiter_gain_for_last_second=1.0f;
  minimum_limiter_gain_for_last_second_pending=1.0f;
  move_to_ex_x=0;
  move_to_ex_y=0;
  select_dc_col=white_c;
  select_dc_brush_col=white_c;
}



void compress_her_limit_her::take_limiter_snapshot(){
  *limiter_snapshot=*limiter;
}



void compress_her_limit_her::set_global_parameters_low(float compressor_threshold_dB,
                                                       float compressor_softness,
                                                       float compressor_ratio,
                                                       float compressor_attack,
                                                       float compressor_low_release,
                                                       float limiter_threshold_dB,
                                                       float limiter_softness,
                                                       float compressor_makeup_gain,
                                                       bool i_am_the_final_limiter_section){
  if (compressor_threshold_dB>last_compressor_threshold_dB || limiter_threshold_dB>last_limiter_threshold_dB || compressor_ratio<last_compressor_ratio){
    changed_parameters_so_release_limiter_counter=LIMITER_RELEASE_TIME_AFTER_PARAMETER_CHANGE;
  }
//  if (compressor_threshold_dB!=last_compressor_threshold_dB ||
//      compressor_softness!=last_compressor_softness ||
//      compressor_ratio!=last_compressor_ratio ||
//      compressor_attack!=last_compressor_attack ||
//      compressor_low_release!=last_compressor_low_release ||
//      limiter_threshold_dB!=last_limiter_threshold_dB ||
//      limiter_softness!=last_limiter_softness ||
//      gain!=last_gain){
//        changed_parameters_so_release_limiter_counter=LIMITER_RELEASE_TIME_AFTER_PARAMETER_CHANGE;
//  }
  last_compressor_threshold_dB=compressor_threshold_dB;
  last_compressor_softness=compressor_softness;
  last_compressor_ratio=compressor_ratio;
  last_compressor_attack=compressor_attack;
  last_compressor_low_release=compressor_low_release;
  last_limiter_threshold_dB=limiter_threshold_dB;
  last_limiter_softness=limiter_softness;
  last_gain=gain;

  compressor->start_of_softknee=compressor_threshold_dB;
  compressor->knee_radius=compressor_softness;
  compressor->ratio=compressor_ratio;
  gain=powf(10.0f,compressor_makeup_gain*0.1f);
  attack=compressor_attack*44.1f;
  release=compressor_low_release*44.1f*7.0f;
  limiter->knee_radius=limiter_softness;
  limiter->limit=limiter_threshold_dB;
  compressor->recalc_params();
  if (!i_am_the_final_limiter_section){
    limiter->recalc_params();
  }else{
    static bool first_pass_so_recalc_final_limiter_things=true;
    if (first_pass_so_recalc_final_limiter_things){
      limiter_snapshot=new soft_limiter;
      limiter->recalc_params();
      *limiter_snapshot=*limiter;
    }
    first_pass_so_recalc_final_limiter_things=false;
    need_to_recalc_params_and_use_atomics=true;
  }
  graph_start_dB=-40.0f;
  graph_end_dB=20.0f;
  comp_EMA_alpha=2.0f/(attack+1.0f);
  compressor_release_rate=2.0f/(release+1.0f);
  limiter_normal_softknee_threshold=powf(10.0f,limiter->start_of_softknee*0.1f);
  limiter_release_rate=2.0f/(LIMITER_RELEASE_TIME*44100.0f*7.0f+1.0f);
}



void compress_her_limit_her::m(int lx,int ly){
  move_to_ex_x=(int)x+lx;
  move_to_ex_y=(int)y-ly+(int)height;
  //  MoveToEx(dc,(int)x+lx,(int)y-ly+(int)height,0);
  //  last_GDI_like_line_operation_was_a_movetoex=true;
}



void compress_her_limit_her::l(int lx,int ly){
  s32 xlt=(int)x+lx;
  s32 ylt=(int)y-ly+(int)height;
  dc->line(move_to_ex_x, move_to_ex_y, xlt, ylt, select_dc_col);
  //  LineTo(dc,(int)x+lx,(int)y+(int)height-ly);
  move_to_ex_x=xlt;
  move_to_ex_y=ylt;
}



void compress_her_limit_her::rec(RTTXCB* dc,int lx,int ly,int lw,int lh){
  m(lx,ly);
  l(lx+lw,ly);
  l(lx+lw,ly+lh);
  l(lx,ly+lh);
  l(lx,ly);
}



void compress_her_limit_her::doit( float &l,float &r,bool update_max_output,bool final_limiter_only){
  if (!final_limiter_only){
    float fl=fabsf(l);
    float fr=fabsf(r);
    instantaneous_max_val=max(fl,fr);
    max_val=max_val*(1.0f-comp_EMA_alpha)+instantaneous_max_val*comp_EMA_alpha;
    if (max_val>1e-5f){
      float compressor_gain=1.0f;
      max_val_dBs=10.0f*log10fRTT(max_val);
      float max_val_dBs_compressed=compressor->doit(max_val_dBs);
      float max_val_compressed=powf(10.0f,max_val_dBs_compressed*0.1f);
      if (bypass){
        compressor_gain=1.0f;
      }else{
        compressor_gain=max_val_compressed/max_val;
      }
      current_compression=max_val_dBs-max_val_dBs_compressed;
      max_current_compression_dbs=max(max_current_compression_dbs,current_compression);
      if (current_dynamics_gain<1.0f){
        current_dynamics_gain+=current_dynamics_gain*compressor_release_rate;
      }
      current_dynamics_gain=min(current_dynamics_gain,compressor_gain);
      assert(compressor_gain<1.01f);
    }

    float gg=current_dynamics_gain*gain;
    min_current_dynamics_reduction=min(min_current_dynamics_reduction,current_dynamics_gain);
    l*=gg;
    r*=gg;
  }

  float fl=fabsf(l);
  float fr=fabsf(r);
  float max_val=max(fl,fr);
  if (update_max_output){
    max_output=max(max_output,max_val);
  }
  float limiter_gain=1.0f;
  current_limitation=0;
  if (max_val>=limiter_normal_softknee_threshold){
    float dBs=10.0f*log10fRTT(max_val);
    float dBs_after_limiting=limiter->doit(dBs);
    float limiter_out=powf(10.0f,dBs_after_limiting*0.1f);
    current_limitation=dBs-dBs_after_limiting;
    limiter_gain=limiter_out/max_val;
  }
  if (bypass){
    limiter_gain=1.0f;
  }
  assert(limiter_gain<1.01f);
  current_limiter_gain=min(current_limiter_gain,limiter_gain);
  current_limiter_gain=min(current_limiter_gain,limiter_hold_gain);
  assert(current_limiter_gain<1.01f);
  float l_test=l*current_limiter_gain;
  float r_test=r*current_limiter_gain;
#ifdef _DEBUG
  if (test_for_clipping){
#define SCAY_LING_FROM_ASIO_INT_INPUT ((double)pow(2.0,31.0)-1.0)
    //  assert(asioDriverInfo.channelInfos[i].type==ASIOSTInt32LSB);
    double lim=SCAY_LING_FROM_ASIO_INT_INPUT;
    double volumevolume_smaller=volumevolume;
    double ld=(double)l_test*lim*volumevolume_smaller;
    double q=abs(ld)/pow(2.0,31.0);
    assert(q<1.00001);
    double rd=(double)r_test*lim*volumevolume_smaller;
    q=abs(rd)/pow(2.0,31.0);
    assert(q<1.00001);
  }
#endif

  l=l_test;
  r=r_test;
  //  {
  //    float max_val=max(fl,fr);
  //    float limiter_gain=1.0f;
  //    current_limitation=0;
  //    float RTTRTTRRTT;
  //    float RTTRTTRRTTdbss;
  //    if (max_val>=limiter_normal_softknee_threshold){
  //      float dBs=10.0f*log10f(max_val);
  //      RTTRTTRRTTdbss=dBs;
  //      float dBs_after_limiting=limiter->doit(dBs);
  //      RTTRTTRRTT=dBs_after_limiting;
  //      float limiter_out=powf(10.0f,dBs_after_limiting*0.1f);
  //      current_limitation=dBs-dBs_after_limiting;
  //      limiter_gain=limiter_out/max_val;
  //    }
  //    if (bypass){
  //      limiter_gain=1.0f;
  //    }
  //    assert(limiter_gain<1.01f);
  //    current_limiter_gain=min(current_limiter_gain,limiter_gain);
  //    current_limiter_gain=min(current_limiter_gain,limiter_hold_gain);
  //    assert(current_limiter_gain<1.01f);
  ////    l*=current_limiter_gain;
  ////    r*=current_limiter_gain;
  //    if (!bypass){
  //      assert(fabsf(l)<=bungtronic_dog_ponies*1.00001f);
  //      assert(fabsf(r)<=bungtronic_dog_ponies*1.00001f);
  //    }
  //    if (fuck_the_knob){
  //#define SCAY_LING_FROM_ASIO_INT_INPUT ((double)pow(2.0,31.0)-1.0)
  //      //  assert(asioDriverInfo.channelInfos[i].type==ASIOSTInt32LSB);
  //      double lim=SCAY_LING_FROM_ASIO_INT_INPUT;
  //      double volumevolume_smaller=volumevolume;
  //      double ld=(double)l*lim*volumevolume_smaller;
  //      double q=abs(ld)/pow(2.0,31.0);
  //      assert(q<1.00001);
  //      double rd=(double)r*lim*volumevolume_smaller;
  //      q=abs(rd)/pow(2.0,31.0);
  //      assert(q<1.00001);
  //    }
  //  }
  minimum_limiter_gain_for_last_second_pending=min(minimum_limiter_gain_for_last_second_pending,limiter_gain);

  if (current_limitation>LIMITER_ACTIVATED_THRESHOLD){
    if (limiter_hold_activation_counter>0 || limiter_hold_ON_countdown>0){
      limiter_hold_ON_countdown=LIMITER_HOLD_TIME;
      if (limiter_gain>limiter_hold_gain){
        limiter_hold_gain=limiter_gain;
        reset_minimum_limiter_gain_for_last_second_counter=RECHECK_CURRENT_LIMITER_LIMITATION_COUNTER;
      }
    }
    limiter_hold_activation_counter=LIMITER_TIME_TO_WAIT_BEFORE_ACTIVIATING_LIMITER_HOLD;
  }

  reset_minimum_limiter_gain_for_last_second_counter--;
  if (reset_minimum_limiter_gain_for_last_second_counter<=0){
    reset_minimum_limiter_gain_for_last_second_counter=RECHECK_CURRENT_LIMITER_LIMITATION_COUNTER;
    minimum_limiter_gain_for_last_second=minimum_limiter_gain_for_last_second_pending;
    minimum_limiter_gain_for_last_second_pending=1.0f;
  }
  if (limiter_hold_gain<minimum_limiter_gain_for_last_second || current_limiter_gain<minimum_limiter_gain_for_last_second){
    limiter_hold_gain=limiter_hold_gain*(1.0f-LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE)+LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE;
    current_limiter_gain=current_limiter_gain*(1.0f-LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE)+LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE;
  }

  if (changed_parameters_so_release_limiter_counter>0){
    // after control change allow things to release
    changed_parameters_so_release_limiter_counter--;
    limiter_hold_gain=limiter_hold_gain*(1.0f-LIMITER_HOLD_PARAMETER_RELEASE_FACTOR)+LIMITER_HOLD_PARAMETER_RELEASE_FACTOR;
    if (current_limiter_gain<1.0f){
      current_limiter_gain=current_limiter_gain*(1.0f-LIMITER_HOLD_PARAMETER_RELEASE_FACTOR)+LIMITER_HOLD_PARAMETER_RELEASE_FACTOR;
    }
  }
  if (limiter_hold_activation_counter>0){
    limiter_hold_activation_counter--;
  }
  if (limiter_hold_ON_countdown>0){
    limiter_hold_ON_countdown--;
  }else{
    limiter_hold_gain=1.0f;
    if (current_limiter_gain<1.0f){
      current_limiter_gain+=current_limiter_gain*limiter_release_rate;
    }
  }
  min_current_limiter_reduction=min(min_current_limiter_reduction,current_limiter_gain);
}







void compress_her_limit_her::doit_comrpessor_only(float &lc,float &rc){
  float fl=fabsf(lc);
  float fr=fabsf(rc);
  instantaneous_max_val=max(fl,fr);
  max_val=max_val*(1.0f-comp_EMA_alpha)+instantaneous_max_val*comp_EMA_alpha;
  if (max_val>1e-5f){
    float compressor_gain=1.0f;
    max_val_dBs=10.0f*log10fRTT(max_val);
    float max_val_dBs_compressed=compressor->doit(max_val_dBs);
    float max_val_compressed=powf(10.0f,max_val_dBs_compressed*0.1f);
    if (bypass){
      compressor_gain=1.0f;
    }else{
      compressor_gain=max_val_compressed/max_val;
    }
    current_compression=max_val_dBs-max_val_dBs_compressed;
    if (current_dynamics_gain<1.0f){
      current_dynamics_gain+=current_dynamics_gain*compressor_release_rate;
    }
    current_dynamics_gain=min(current_dynamics_gain,compressor_gain);
    assert(compressor_gain<1.01f);
  }

  float gg=current_dynamics_gain;
  max_current_compression_dbs=max(max_current_compression_dbs,current_compression);
  min_current_dynamics_reduction=min(min_current_dynamics_reduction,current_dynamics_gain);
  lc*=gg;
  rc*=gg;
}



void compress_her_limit_her::doit_limiter_only( float &llim,float &rlim,bool update_max_output){
  // limiter
  llim*=gain;
  rlim*=gain;
  float fl=fabsf(llim);
  float fr=fabsf(rlim);
  float max_val=max(fl,fr);
  if (update_max_output){
    max_output=max(max_output,max_val);
  }
  float limiter_gain=1.0f;
  current_limitation=0;
  if (max_val>=limiter_normal_softknee_threshold){
    float dBs=10.0f*log10fRTT(max_val);
    float dBs_after_limiting=limiter->doit(dBs);
    float limiter_out=powf(10.0f,dBs_after_limiting*0.1f);
    current_limitation=dBs-dBs_after_limiting;
    limiter_gain=limiter_out/max_val;
  }
  if (bypass){
    limiter_gain=1.0f;
  }
  assert(limiter_gain<1.01f);
  current_limiter_gain=min(current_limiter_gain,limiter_gain);
  current_limiter_gain=min(current_limiter_gain,limiter_hold_gain);
  assert(current_limiter_gain<1.01f);
  float l_test=llim*current_limiter_gain;
  float r_test=rlim*current_limiter_gain;

  llim=l_test;
  rlim=r_test;



  minimum_limiter_gain_for_last_second_pending=min(minimum_limiter_gain_for_last_second_pending,limiter_gain);

  if (current_limitation>LIMITER_ACTIVATED_THRESHOLD){
    if (limiter_hold_activation_counter>0 || limiter_hold_ON_countdown>0){
      limiter_hold_ON_countdown=LIMITER_HOLD_TIME;
      if (limiter_gain>limiter_hold_gain){
        limiter_hold_gain=limiter_gain;
        reset_minimum_limiter_gain_for_last_second_counter=RECHECK_CURRENT_LIMITER_LIMITATION_COUNTER;
      }
    }
    limiter_hold_activation_counter=LIMITER_TIME_TO_WAIT_BEFORE_ACTIVIATING_LIMITER_HOLD;
  }

  reset_minimum_limiter_gain_for_last_second_counter--;
  if (reset_minimum_limiter_gain_for_last_second_counter<=0){
    reset_minimum_limiter_gain_for_last_second_counter=RECHECK_CURRENT_LIMITER_LIMITATION_COUNTER;
    minimum_limiter_gain_for_last_second=minimum_limiter_gain_for_last_second_pending;
    minimum_limiter_gain_for_last_second_pending=1.0f;
  }
  if (limiter_hold_gain<minimum_limiter_gain_for_last_second || current_limiter_gain<minimum_limiter_gain_for_last_second){
    limiter_hold_gain=limiter_hold_gain*(1.0f-LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE)+LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE;
    current_limiter_gain=current_limiter_gain*(1.0f-LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE)+LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE;
  }

  if (changed_parameters_so_release_limiter_counter>0){
    // after control change allow things to release
    changed_parameters_so_release_limiter_counter--;
    limiter_hold_gain=limiter_hold_gain*(1.0f-LIMITER_HOLD_PARAMETER_RELEASE_FACTOR)+LIMITER_HOLD_PARAMETER_RELEASE_FACTOR;
    if (current_limiter_gain<1.0f){
      current_limiter_gain=current_limiter_gain*(1.0f-LIMITER_HOLD_PARAMETER_RELEASE_FACTOR)+LIMITER_HOLD_PARAMETER_RELEASE_FACTOR;
    }
  }
  if (limiter_hold_activation_counter>0){
    limiter_hold_activation_counter--;
  }
  if (limiter_hold_ON_countdown>0){
    limiter_hold_ON_countdown--;
  }else{
    limiter_hold_gain=1.0f;
    if (current_limiter_gain<1.0f){
      current_limiter_gain+=current_limiter_gain*limiter_release_rate;
    }
  }
  min_current_limiter_reduction=min(min_current_limiter_reduction,current_limiter_gain);
}



float compress_her_limit_her::get_current_gain(){
  return current_dynamics_gain*gain*current_limiter_gain;
}



float compress_her_limit_her::get_plot_coordx(float in){
  float graph_db_width=graph_end_dB-graph_start_dB;
  return COMP_TRANSFER_FN_START_X+(in-graph_start_dB)*COMP_TRANSFER_FN_H/graph_db_width;
}



float compress_her_limit_her::get_plot_coordy(float in){
  float graph_db_width=graph_end_dB-graph_start_dB;
  return COMP_TRANSFER_FN_START_Y+(in-graph_start_dB)*COMP_TRANSFER_FN_H/graph_db_width;
}



void compress_her_limit_her::draw_reference_key_line(float in,float out,bool selpen){
  if (selpen){
    SelectObject(dc,penDot);
  }
  float graph_db_width=graph_end_dB-graph_start_dB;
  float key_x=COMP_TRANSFER_FN_START_X+(in-graph_start_dB)*COMP_TRANSFER_FN_H/graph_db_width;
  float key_y=COMP_TRANSFER_FN_START_Y+(out-graph_start_dB)*COMP_TRANSFER_FN_H/graph_db_width;
  m((int)key_x,COMP_TRANSFER_FN_START_Y);
  l((int)key_x,(int)key_y);
  l(COMP_TRANSFER_FN_START_X,(int)key_y);
}



void compress_her_limit_her::text_n_tick_y_axis(float v,const char*txt,COLORREF ctext,COLORREF cnum,COLORREF cother){
  int y2=(int)(get_plot_coordy(v)+0.5f);
  SetDCPenColor2(dc,grey25_c);
  m(COMP_TRANSFER_FN_START_X,y2);
  l(COMP_TRANSFER_FN_START_X-3,y2);
  print_pretty_right_justify(dc,(int)(x+(float)COMP_TRANSFER_FN_START_X-3.5f),(int)(y+height-y2-1.5f),txt,ctext,cnum,cother);
}



void compress_her_limit_her::text_n_tick_y_axis_inside_graph(float v,const char*txt,COLORREF ctext,COLORREF cnum,COLORREF cother){
  int y2=(int)(get_plot_coordy(v)+0.5f);
  SetDCPenColor2(dc,grey25_c);
  m(COMP_TRANSFER_FN_START_X+3,y2);
  l(COMP_TRANSFER_FN_START_X,y2);
  print_pretty(dc,(int)(x+(float)COMP_TRANSFER_FN_START_X+1.5f),(int)(y+height-y2+4.5f),txt,ctext,cnum,cother);
}



void compress_her_limit_her::text_n_tick_x_axis(float v,const char*txt,COLORREF ctext,COLORREF cnum,COLORREF cother){
//  int y2=(int)(get_plot_coordy(v)+0.5f);
//  SetDCPenColor2(dc,grey25_c);
//  m(COMP_TRANSFER_FN_START_X,y2);
//  l(COMP_TRANSFER_FN_START_X-2,y2);

//  int x2=(int)(get_plot_coordx(0)+0.5f);
  int y2=COMP_TRANSFER_FN_START_Y-6;
  int x2=(int)(get_plot_coordx(v)+0.5f);
  m(x2,COMP_TRANSFER_FN_START_Y);
  l(x2,COMP_TRANSFER_FN_START_Y-3);
  print_pretty(dc,(int)(x+x2-3.5f),(int)(y+height-y2-2.5f),txt,ctext,cnum,cother);
//  print_pretty(dc,(int)(x+x2+0.5f),(int)(y+height-((float)COMP_TRANSFER_FN_START_Y-3)+0.5f),"0dB in",grey65_c,coral_c,white_c);
}



//void compress_her_limit_her::render(RTTXCB* rttxcb){
//  draw_LED_display_background_into_cached_bitmap(rttxcb);
//  float ox=x;
//  float oy=y;
//  //  if (buttmap==0){
//  //    buttmap=CreateCompatibleBitmap(dc, (int)width, (int)height);
//  //    bitmap_dc=CreateCompatibleDC(dc);
//  //    SelectObject(bitmap_dc,buttmap);
//  //  }
//  x=0;
//  y=0;
//  draw(&rttxcb);
//  x=ox;
//  y=oy;
//  ready_to_draw=true;
//}




bool compress_her_limit_her::test_rect_for_overlap(RECT&ur){
  if (!ready_to_draw) return false;
  if (ur.right<(int)x) return false;
  if (ur.left>(int)(x+width)) return false;
  if (ur.bottom<(int)y) return false;
  if (ur.top>(int)(y+height)) return false;
  return true;
}



//void compress_her_limit_her::draw_compressor_pic_from_cached(RTTXCB* dc){
//  if (!bitmap_dc){
//    draw_into_cached_bitmap(dc);
//  }
//  if (BitBlt(dc,(int)x,(int)y,(int)width, (int)height,bitmap_dc,0,0,SRCCOPY)==0){
//    assert(0);
//  }
//}



//void compress_her_limit_her::render(RTTXCB* rttxcb){
//  draw_LED_display_background(rttxcb);
//  draw_LED_display(rttxcb);
//  draw(rttxcb);
//}
  
  



//void compress_her_limit_her::draw_LEDs_from_cached(RTTXCB* dc){
//  if (BitBlt(dc,(int)x,(int)y,(int)width, (int)height,bitmap_dc,0,0,SRCCOPY)==0){
//    assert(0);
//  }
//  RECT orect=LED_rect;
//  int LED_w=LED_rect.right-LED_rect.left;
//  int LED_h=LED_rect.bottom-LED_rect.top;
//  LED_rect.left=0;
//  LED_rect.right=LED_w;
//  LED_rect.top=0;
//  LED_rect.bottom=LED_h;
//  draw_LED_display_background_from_cached(temp_bitmap_LED_dc);
//  draw_LED_display(temp_bitmap_LED_dc);
//  LED_rect=orect;
//  BitBlt(dc,LED_rect.left,LED_rect.top,LED_w, LED_h,temp_bitmap_LED_dc,0,0,SRCCOPY);
//}



int compression_levels[NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS]={21,18,15,12,10,8,6,5,4,3,2,1};
int gain_levels[NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS]={-18,-12,-6,-3,0,2,4,6,8,10,12,15,18};

#define GAIN_DISPLAY_dBS_OFFSET 10.0f


//current_compression
//current_rms_dBs

#define IN_OR_OUT_METER_TEXT_OFFSET 2

//void compress_her_limit_her::draw_LED_display_background_into_cached_bitmap(RTTXCB* dc){
//  RECT orect=LED_rect;
//  float ox=(float)LED_rect.left;
//  float oy=(float)LED_rect.bottom;
//  int LED_w=LED_rect.right-LED_rect.left;
//  int LED_h=LED_rect.bottom-LED_rect.top;
//  if (buttmap_LED==0){
//    //    buttmap_LED=CreateCompatibleBitmap(dc, LED_w,LED_h);
//    //    bitmap_LED_dc=CreateCompatibleDC(dc);
//    //    temp_buttmap_LED=CreateCompatibleBitmap(dc, LED_w,LED_h);
//    //    temp_bitmap_LED_dc=CreateCompatibleDC(dc);
//    //    SelectObject(bitmap_LED_dc,buttmap_LED);
//    //    SelectObject(temp_bitmap_LED_dc,temp_buttmap_LED);
//  }
//
//  LED_rect.left=0;
//  LED_rect.right=LED_w;
//  LED_rect.top=0;
//  LED_rect.bottom=LED_h;
//  draw_LED_display_background(bitmap_LED_dc);
//  LED_rect=orect;
//}



//void compress_her_limit_her::draw_LED_display_background_from_cached(RTTXCB* dc){
//  //  if (BitBlt(dc,LED_rect.left,LED_rect.top,LED_rect.right-LED_rect.left, LED_rect.bottom-LED_rect.top,bitmap_LED_dc,0,0,SRCCOPY)==0){
//  //    assert(0);
//  //  }
//}



void compress_her_limit_her::draw_LED_display_background(RTTXCB* dc){
  //  SelectStockObject2(dc,DC_BRUSH);
  SetDCPenColor2(dc,grey15_c);
  SetDCBrushColor2(dc,grey15_c);
  Rectangle(dc,LED_rect.left,LED_rect.top,LED_rect.right,LED_rect.bottom);

  int x=COMPRESSOR_LED_START+LED_rect.left;
  int y=LED_rect.top+COMPRESSOR_LED_Y_GAP;
  SetDCPenColor2(dc,black_c);
  SetDCBrushColor2(dc,black_c);
  Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  print_colour(dc,x+IN_OR_OUT_METER_TEXT_OFFSET,y-7,"-",grey80_c,true);
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  print_colour(dc,x+IN_OR_OUT_METER_TEXT_OFFSET,y-7,"0",grey80_c,true);
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  print_colour(dc,x+IN_OR_OUT_METER_TEXT_OFFSET,y-7,"+",grey80_c,true);
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  int ox=x;
  x=COMPRESSOR_LED_START+LED_rect.left;
  y+=COMPRESSOR_LED_HEIGHT+COMPRESSOR_LED_GAP;
  Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  y-=COMPRESSOR_LED_Y_GAP;
  x=ox;

  x+=COMPRESSOR_LED_GAP;
  ox=x;
  SetDCPenColor2(dc,black_c);
  SetDCBrushColor2(dc,black_c);
  for(int i=0;i<NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS;i++){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    char rtt[8];
    sprintf(rtt,"%d",compression_levels[i]);
    print_colour(dc,x,y-7,rtt,grey80_c,true);
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }
  for(int i=0;i<NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS;i++){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    char rtt[8];
    sprintf(rtt,"%d",gain_levels[i]);
    print_colour_right_justify(dc,x+COMPRESSOR_LED_WIDTH-3,y-7,rtt,grey80_c,true);
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }

  y+=COMPRESSOR_LED_HEIGHT+COMPRESSOR_LED_GAP;
  x=ox;
  for(int i=0;i<NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS;i++){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }
  for(int i=0;i<NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS;i++){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }

  x+=LIMITER_DISPLAY_OFFSET;
  y=LED_rect.top+1;
  for(int i=0;i<4;i++){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    switch(i){
      case 0:print_colour_right_justify(dc,x-3,y,"-12",white_c,true);break;
      case 1:print_colour_right_justify(dc,x-3,y,"-6",white_c,true);break;
      case 2:print_colour_right_justify(dc,x-3,y,"-3",white_c,true);break;
      case 3:print_colour_right_justify(dc,x-3,y,"-.1",white_c,true);break;
    }
    y+=COMPRESSOR_LED_HEIGHT+1;
  }
}



void compress_her_limit_her::draw_LED_display(RTTXCB* dc){
  int x=COMPRESSOR_LED_START+LED_rect.left;
  int y=LED_rect.top+COMPRESSOR_LED_Y_GAP;
  float current_rms_dBs=10.0f*log10fRTT(instantaneous_max_val);

  if (current_rms_dBs>=compressor->end_of_softknee && compressor->ratio!=1.0f){
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
    if (bypass && synchronised_flash_frame){
//      SetDCPenColor2(dc,black_c);
//      SetDCBrushColor2(dc,black_c);
    }else{
      SetDCPenColor2(dc,red_c);
      SetDCBrushColor2(dc,red_c);
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }else if (current_rms_dBs>=compressor->start_of_softknee && compressor->ratio!=1.0f){
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
    if (bypass && synchronised_flash_frame){
//      SetDCPenColor2(dc,black_c);
//      SetDCBrushColor2(dc,black_c);
    }else{
      SetDCPenColor2(dc,orange_c);
      SetDCBrushColor2(dc,orange_c);
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }else{
    if (bypass && synchronised_flash_frame){
//      SetDCPenColor2(dc,black_c);
//      SetDCBrushColor2(dc,black_c);
    }else{
      SetDCPenColor2(dc,green_c);
      SetDCBrushColor2(dc,green_c);
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }

  int ox=x;
  x=COMPRESSOR_LED_START+LED_rect.left;
  y+=COMPRESSOR_LED_HEIGHT+COMPRESSOR_LED_GAP;
  if (!limiter_hold_activation_counter && !limiter_hold_ON_countdown){
    SetDCPenColor2(dc,green_c);
    SetDCBrushColor2(dc,green_c);
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  if (limiter_hold_activation_counter){
    SetDCPenColor2(dc,orange_c);
    SetDCBrushColor2(dc,orange_c);
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  if (limiter_hold_ON_countdown){
    SetDCPenColor2(dc,red_c);
    SetDCBrushColor2(dc,red_c);
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  y-=COMPRESSOR_LED_Y_GAP;
  x=ox;

  x+=COMPRESSOR_LED_GAP;
  int comp=(int)(max_current_compression_dbs+.5f);
  ox=x;
  if (bypass){
    if (synchronised_flash_frame){
      SetDCPenColor2(dc,black_c);
      SetDCBrushColor2(dc,black_c);
    }else{
      SetDCPenColor2(dc,red4_c);
      SetDCBrushColor2(dc,red4_c);
    }
  }else{
    SetDCPenColor2(dc,red_c);
    SetDCBrushColor2(dc,red_c);
  }

  for(int i=0;i<NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS;i++){
    if (compression_levels[i]<=comp){
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }
  int peak_input=(int)(current_rms_dBs+0.5f);
  SetDCPenColor2(dc,green_c);
  SetDCBrushColor2(dc,green_c);
  for(int i=0;i<NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS;i++){
    if (gain_levels[i]<=peak_input){
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }

  y+=COMPRESSOR_LED_HEIGHT+COMPRESSOR_LED_GAP;
  x=ox;
  float current_limitation2=-10.0f*log10fRTT(min_current_limiter_reduction);
  comp=(int)(current_limitation2-10.0f*log10fRTT(min_current_dynamics_reduction)+0.5f);
  SetDCPenColor2(dc,orange3_c);
  SetDCBrushColor2(dc,orange3_c);
  for(int i=0;i<NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS;i++){
    if (compression_levels[i]<=current_limitation2){
      SetDCPenColor2(dc,red_c);
      SetDCBrushColor2(dc,red_c);
    }
    if (compression_levels[i]<=comp){
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }
  int peak_output=(int)(10.0f*log10fRTT(instantaneous_max_val*min_current_dynamics_reduction*min_current_limiter_reduction*gain)+0.5f);
  kokkok=1e-20f;
  SetDCPenColor2(dc,green_c);
  SetDCBrushColor2(dc,green_c);
  for(int i=0;i<NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS;i++){
    if (gain_levels[i]<=peak_output){
      Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
    }
    x+=COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP;
  }

  SetDCPenColor2(dc,red_c);
  SetDCBrushColor2(dc,red_c);
  x+=LIMITER_DISPLAY_OFFSET;
  y=LED_rect.top+1;
  if (current_limitation2>11.0f){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  y+=COMPRESSOR_LED_HEIGHT+1;
  if (current_limitation2>5.5f){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  y+=COMPRESSOR_LED_HEIGHT+1;
  if (current_limitation2>2.8f){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  y+=COMPRESSOR_LED_HEIGHT+1;
  if (current_limitation2>0.1f){
    Rectangle(dc,x,y,x+COMPRESSOR_LED_WIDTH,y+COMPRESSOR_LED_HEIGHT);
  }
  max_current_compression_dbs=0;
  min_current_dynamics_reduction=1.0f;
  min_current_limiter_reduction=1.0f;
}



void compress_her_limit_her::draw(RTTXCB& _dc){
  dc=&_dc;
  //  SetBkColor(dc,grey15_c);
  //  SelectStockObject2(dc,DC_BRUSH);
  SetDCPenColor2(dc,grey15_c);
  SetDCBrushColor2(dc,grey15_c);
  Rectangle(dc,(int)x,(int)y,(int)(x+width),(int)(y+height));
  SetDCPenColor2(dc,grey25_c);
  rec(dc,COMP_TRANSFER_FN_START_X,COMP_TRANSFER_FN_START_Y,COMP_TRANSFER_FN_W,COMP_TRANSFER_FN_H);
  SelectObject(dc,penDotzero_db_in);

  float graph_db_width=graph_end_dB-graph_start_dB;
  float zero_db_out_y=COMP_TRANSFER_FN_H*(-graph_start_dB)/graph_db_width+COMP_TRANSFER_FN_START_Y;
  m(COMP_TRANSFER_FN_START_X,(int)zero_db_out_y);
  l(COMP_TRANSFER_FN_START_X+COMP_TRANSFER_FN_W,(int)zero_db_out_y);
  SelectObject(dc,Pen2green_pen);
  COLORREF oth=grey35_c;
  text_n_tick_y_axis(20.0f,"+20",grey35_c,oth,oth);
  text_n_tick_y_axis(10.0f,"+10",grey35_c,oth,oth);
  text_n_tick_y_axis(0.0f,"0",grey35_c,oth,oth);
  text_n_tick_y_axis(-10.0f,"-10",grey35_c,oth,oth);
  text_n_tick_y_axis(-20.0f,"-20",grey35_c,oth,oth);
  text_n_tick_y_axis(-30.0f,"-30",grey35_c,oth,oth);
  text_n_tick_y_axis(-40.0f,"-40",grey35_c,oth,oth);

//  text_n_tick_x_axis(60.0f,"+60",grey35_c,oth,oth);
  text_n_tick_x_axis(40.0f,"+40",grey35_c,oth,oth);
//  text_n_tick_x_axis(20.0f,"+30",grey35_c,oth,oth);
  text_n_tick_x_axis(20.0f,"+20",grey35_c,oth,oth);
//  text_n_tick_x_axis(10.0f,"+10",grey35_c,oth,oth);
  text_n_tick_x_axis(0.0f,"0",grey35_c,oth,oth);
//  text_n_tick_x_axis(-10.0f,"-10",grey35_c,oth,oth);
  text_n_tick_x_axis(-20.0f,"-20",grey35_c,oth,oth);
//  text_n_tick_x_axis(-30.0f,"-30",grey35_c,oth,oth);
  text_n_tick_x_axis(-40.0f,"-40",grey35_c,oth,oth);

  print_colour(dc,(int)x+3,(int)y+3,desc,grey65_c,true);



  bool drawn_limiter_start=false;
  bool drawn_limiter_end=false;
  float lastin=graph_start_dB;
  int xplot[COMP_TRANSFER_FN_W+1];
  int yplot[COMP_TRANSFER_FN_W+1];
  int col_to_plot[COMP_TRANSFER_FN_W+1];
  int col=0;
  for(int i=0;i<=COMP_TRANSFER_FN_W;i+=2){
    float in=graph_start_dB+graph_db_width*(float)i/COMP_TRANSFER_FN_H;
    float v=compressor->doit(in);
    v=limiter_snapshot->doit(v);
    if (lastin<0 && in>=0){
      SelectObject(dc,penDotzero_db_in);
      draw_reference_key_line(0,v,false);
      SetDCPenColor2(dc,white_c);
      int x2=(int)(get_plot_coordx(0)+0.5f);
      m(x2,COMP_TRANSFER_FN_START_Y+1);
      l(x2,COMP_TRANSFER_FN_START_Y+3);
      print_pretty(dc,(int)(x+x2+0.5f),(int)(y+height-((float)COMP_TRANSFER_FN_START_Y-3)-8.5f),"0dB",grey65_c,coral_c,white_c);
      char rtt[32];
      sprintf(rtt,"%ddB",(int)(v+0.5f));
      text_n_tick_y_axis_inside_graph(v, rtt, grey65_c, coral_c, white_c);
    }
    lastin=in;
    if (compressor->on_knee){
      col=1;
    }
    if (compressor->compressing){
      col=2;
    }
    if (limiter_snapshot->on_knee){
      if (!drawn_limiter_start){
//        draw_reference_key_line(in,v);
        drawn_limiter_start=true;
      }
      col=3;
    }
    if (limiter_snapshot->compressing){
      if (!drawn_limiter_end){
//        draw_reference_key_line(in,v);
        drawn_limiter_end=true;
      }
      col=4;
    }
    col_to_plot[i]=col;
    float vl=v;
    float yp=(vl-graph_start_dB)*(float)COMP_TRANSFER_FN_H/(graph_end_dB-graph_start_dB);
    xplot[i]=(int)((float)COMP_TRANSFER_FN_START_X+(float)i+0.5f);
    yplot[i]=(int)((float)COMP_TRANSFER_FN_START_Y+yp+0.5f);
  }
  m(COMP_TRANSFER_FN_START_X,COMP_TRANSFER_FN_START_Y);
  for(int i=0;i<=COMP_TRANSFER_FN_W;i+=2){
    switch(col_to_plot[i]){
      case 0:SelectObject(dc,Pen2green_pen);break;
      case 1:SelectObject(dc,Pen2yellow_pen);break;
      case 2:SelectObject(dc,Pen2orange_pen);break;
      case 3:SelectObject(dc,Pen2red_pen);break;
      case 4:SelectObject(dc,Pen2white_pen);break;
    }
    l(xplot[i],yplot[i]);
  }
  needs_redraw=false;
}


