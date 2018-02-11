#pragma once

#define NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS 12
#define NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS 13

#define COMPRESSOR_LED_WIDTH 11
#define COMPRESSOR_LED_HEIGHT 5
#define COMPRESSOR_LED_GAP 3
#define COMPRESSOR_LED_START 3
#define COMPRESSOR_LED_END 0
#define COMPRESSOR_LED_Y_GAP 8

#define LIMITER_DISPLAY_OFFSET 10
//#define COMPRESSOR_LED_DISPLAY_WIDTH (LIMITER_DISPLAY_OFFSET+COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_END+(NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS+NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS+3)*(COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP)+3*COMPRESSOR_LED_GAP+COMPRESSOR_LED_START*2-COMPRESSOR_LED_GAP)
#define COMPRESSOR_LED_DISPLAY_WIDTH (COMPRESSOR_LED_START+(COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP)*3+COMPRESSOR_LED_GAP+  (COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_GAP)*(NUM_COMPRESSOR_LED_GAIN_DISPLAY_LEVELS+NUM_COMPRESSOR_LED_COMPRESSION_DISPLAY_LEVELS)+COMPRESSOR_LED_GAP+LIMITER_DISPLAY_OFFSET+COMPRESSOR_LED_WIDTH+COMPRESSOR_LED_END)

#define COMPRESSOR_LED_DISPLAY_HEIGHT (COMPRESSOR_LED_HEIGHT*2+COMPRESSOR_LED_Y_GAP+COMPRESSOR_LED_GAP+3)


#define NUM_COMPRESSOR_ATTACK_TIMES 41
#define NUM_COMPRESSOR_RELEASE_TIMES 41
#define NUM_COMPRESSOR_RATIOS 21
#define NUM_COMPRESSOR_KNEE_RADII 11


#define COMP_TRANSFER_FN_START_X 15
#define COMP_TRANSFER_FN_START_Y 10
#define COMP_TRANSFER_FN_W 110
#define COMP_TRANSFER_FN_H 67
#define COMP_TRANSFER_FN_END_X 4
#define COMP_TRANSFER_FN_END_Y 16

#define LIMITER_RELEASE_TIME 0.1f

// limiter hold parameters
#define LIMITER_ACTIVATED_THRESHOLD 0.25f
#define LIMITER_TIME_TO_WAIT_BEFORE_ACTIVIATING_LIMITER_HOLD 882  // 20ms at 44.1khz
#define LIMITER_HOLD_TIME 44100  // hold for this many samplesif more than 1 limitation occured in the last LIMITER_TIME_TO_WAIT_BEFORE_ACTIVIATING_LIMITER_HOLD samples
#define LIMITER_HOLD_PARAMETER_RELEASE_FACTOR (1.0f/(2.0f*44100.0f))
#define LIMITER_HOLD_GAIN_SIGNAL_TRACK_RATE (1.0f/(2.0f*44100.0f))
#define LIMITER_RELEASE_TIME_AFTER_PARAMETER_CHANGE 22050

#define RECHECK_CURRENT_LIMITER_LIMITATION_COUNTER 44100


class soft_compressor{
public:
  soft_compressor(float _compressor_trigger_level,float _ratio, float _knee_radius);
  void recalc_params();
  float circle_y_as_fn_of_x(float x);
  float doit(float in);
  float y_at_end_of_softknee;
  float knee_radius;
  float ratio;
  float start_of_softknee;
  float end_of_softknee;
  bool compressing;
  bool on_knee;
  float knee_circle_origin_x;
  float knee_circle_origin_y;
};



class soft_limiter{
public:
  soft_limiter(){};
  soft_limiter(float _compressor_trigger_level, float _knee_radius);
  void recalc_params();
  float circle_y_as_fn_of_x(float x);
  float doit(float in);
  float y_at_end_of_softknee;
  float knee_radius;
  float start_of_softknee;
  float end_of_softknee;
  bool compressing;
  bool on_knee;
  float knee_circle_origin_x;
  float knee_circle_origin_y;
  float limit;
  bool avoid_redrawing_me_yet;
};



class compress_her_limit_her{
public:
  explicit compress_her_limit_her(RTTXCB &_rttxcb, const char *_desc,float _x, float _y, float _graph_width,float _graph_height,RECT &_LED_rect);
  void doit(float &l,float &r,bool update_max_output,bool final_limiter_only=false);
  void doit_separate_comrpessor_and_limiter( float &llim,float &rlim,float &lc,float &rc,bool update_max_output,bool final_limiter_only);
  void doit_comrpessor_only(float &lc,float &rc);
  void doit_limiter_only( float &llim,float &rlim,bool update_max_output);
  void set_params(float _compressor_trigger_level,float _ratio, float _knee_sharpness,float _limiter_level,float _limiter_sharpness);
  void set_global_parameters_low(float compressor_threshold_dB,float compressor_softness,float compressor_ratio,float compressor_attack,float compressor_low_release,float limiter_threshold_dB,float limiter_softness,float output_makeup_gain,bool i_am_the_final_limiter_section=false);
  float cubic_interpolation(float p1, float p2, float p3, float p4, float t);
  void draw(RTTXCB& dc);
  void draw_reference_key_line(float in,float out,bool selpen=true);
  bool test_rect_for_overlap(RECT&ur);

  //  void render(RTTXCB* rttxcb);

  RTTXCB &rttxcb;
  const char *desc;
  bool bypass;
  RTTXCB* dc;
  float gain;
  float limiter_normal_softknee_threshold;
  bool ready_to_draw;
  float boochong_koon;
  bool compressing;
  bool limiting;
  float x;
  float y;
  HPEN penDot;
  HPEN penDotzero_db_in;
  HPEN Pen2green_pen;
  HPEN Pen2yellow_pen;
  HPEN Pen2orange_pen;
  HPEN Pen2red_pen;
  HPEN Pen2white_pen;

  static float attack_time[NUM_COMPRESSOR_ATTACK_TIMES];
  static float release_time[NUM_COMPRESSOR_ATTACK_TIMES];
  static float ratios[NUM_COMPRESSOR_RATIOS];
  static float compressor_knee_radius[NUM_COMPRESSOR_KNEE_RADII];
  int limiter_hold_activation_counter;
  int limiter_hold_ON_countdown;
  float limiter_hold_gain;
  float current_compression;
  float max_current_compression_dbs; 
  float current_limitation;
  float minimum_limiter_gain_for_last_second;
  float minimum_limiter_gain_for_last_second_pending;
  int reset_minimum_limiter_gain_for_last_second_counter;
  int changed_parameters_so_release_limiter_counter;
  float last_compressor_threshold_dB;
  float last_compressor_softness;
  float last_compressor_ratio;
  float last_compressor_attack;
  float last_compressor_low_release;
  float last_limiter_threshold_dB;
  float last_limiter_softness;
  float last_gain;

  float max_val_dBs;
  float kokkok;
  void draw_LED_display(RTTXCB* dc);
  RECT LED_rect;
  float attack;
  float instantaneous_max_val;
  float release;
  float comp_EMA_alpha;
  float compressor_release_rate;
  float limiter_release_rate;
  float max_val;
  int rms_counter;
  float gain_tracking_rate;
  float current_dynamics_gain;
  float current_limiter_gain;

  float min_current_dynamics_reduction;
  float min_current_limiter_reduction;

  //  void draw_into_cached_bitmap(RTTXCB* dc);
  //  void draw_LEDs_from_cached(RTTXCB* dc);
  //  void draw_compressor_pic_from_cached(RTTXCB* dc);
  //  void draw_LED_display_background_into_cached_bitmap(RTTXCB* dc);
  //  void draw_LED_display_background_from_cached(RTTXCB* dc);
  void draw_LED_display_background(RTTXCB* dc);
  //  HBITMAP buttmap;
  //  RTTXCB* bitmap_dc;
  //  HBITMAP buttmap_LED;
  //  RTTXCB* bitmap_LED_dc;
  //  HBITMAP temp_buttmap_LED;
  //  RTTXCB* temp_bitmap_LED_dc;
  bool needs_redraw;

  float get_current_gain();
  float get_plot_coordx(float in);
  float get_plot_coordy(float in);
  void text_n_tick_y_axis(float v,const char*txt,COLORREF ctext,COLORREF cnum,COLORREF cother);
  void text_n_tick_y_axis_inside_graph(float v,const char*txt,COLORREF ctext,COLORREF cnum,COLORREF cother);
  void text_n_tick_x_axis(float v,const char*txt,COLORREF ctext,COLORREF cnum,COLORREF cother);
  void take_limiter_snapshot();

  soft_compressor *compressor;
  soft_limiter *limiter;

  soft_limiter *limiter_snapshot;
  float graph_start_dB;
  float graph_end_dB;
  float width;
  float height;
  //  bool last_GDI_like_line_operation_was_a_movetoex;
  void m(int x,int y);
  void l(int x,int y);
  void rec(RTTXCB* dc,int x,int y,int w,int h);

  void SelectObject(RTTXCB* dc,HPEN &pen);
  HPEN CreatePen(s32 style,s32 width,COLORREF col);
  void SetDCPenColor2(RTTXCB* dc,COLORREF col);
  void SetDCBrushColor2(RTTXCB* dc,COLORREF col);
  void Rectangle(RTTXCB* dc,s32 left,s32 top,s32 right,s32 bottom);
  s32 move_to_ex_x;
  s32 move_to_ex_y;
  COLORREF select_dc_col;
  COLORREF select_dc_brush_col;
};

