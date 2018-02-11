#ifndef INCLUDED_RTT_PANE_HEADER
#define INCLUDED_RTT_PANE_HEADER

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <memory>

#include "RTT_colours.h"

using namespace std;
class rtt_window;
class rtt_font;

#define PANE_SPECIAL_FUNCTION_NOTHING 0
#define PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL 1
#define PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_SWITCH 2
#define PANE_SPECIAL_FUNCTION_PAINT_FUNCTION_IS_GENERIC_STD_FUNCTION_TO_CALL 3

#define DONT_RELOAD_FROM_PRESET true
#define RELOAD_FROM_PRESET false

#define TOGGLE_SWITCH_ON 0
#define TOGGLE_SWITCH_OFF 1.0f

#define IS_MOMENTARY_SWITCH true
#define IS_TOGGLE_SWITCH false

#define IS_FLASHING_SWITCH true
#define IS_NONFLASHING_SWITCH false

#define SWITCH_KNOB_BITMAP_NUM_FRAMES 3

class rtt_pane:public RTTXCB{
 public:
  rtt_pane(rtt_window&_parent_window,
           string _name,
           paintfn_type &&_paint_fn,
           clickfn_type &&_click_fn,
           mouse_move_fn_type &&_mouse_move_fn,
           s32 _x,
           s32 _y,
           s32 _w,
           s32 _h,
           bool _render_with_window_scroll_offset=true);
  s32 mouse_click(s32 butt,s32 x,s32 y);
  bool mouse_move(s32 x_mouse,s32 y_mouse);
  void set_extra_bitmap(unique_ptr<RTTXCB> _extra_bitmap){
    extra_bitmap=move(_extra_bitmap);
  }
  void paint();
  paintfn_type paint_fn;
  clickfn_type click_fn;
  u32 pane_special_functionality;
  void add_dial(string name,s32 _x,s32 _y,s32 _start_frame,float _degrees_start, float _degrees_end, string _group,string _desc,bool _dont_reload_me);
  void add_dial(unique_ptr<RTTXCB> &knob_bitmap,s32 _x,s32 _y,s32 _start_frame,float _degrees_start, float _degrees_end,string _group,string _desc,bool _dont_reload_me);
  void add_switch(unique_ptr<RTTXCB> &knob_bitmap,
                  s32 _x,
                  s32 _y,
                  string _group,
                  string _desc,
                  bool is_momentary,
                  bool _is_flashing,
                  bool _dont_reload_me);
  bool handle_knob_dial_display_frame_from_mouse_pos(s32 pane_mouse_x,s32 pane_mouse_y);
  bool handle_knob_mouse_wheel(s32 increment);
  float current_control_setting; // if its a control like a dial this is the value it has
  mouse_move_fn_type mouse_move_fn;
  s32 get_available_text_rendering_width();
  u32 key(u32 key_action,u32 OS_keycode,bool is_ascii,u8 ASCII);
  //  void line(float x,float y);
  rtt_font& get_font();
  rtt_window &parent_window;
  string name;
  s32 x; // position relative to parent window
  s32 y; // width & height are in the bitmap
  bool render_with_window_scroll_offset;
  bool text_bold;
  COLORREF text_colour;
  bool mouse_was_clicked_inside_pane;
  bool do_a_refresh;
  unique_ptr<RTTXCB> extra_bitmap;

  float knob_float_to_change_to_reflect_current_setting;
  s32 knob_frame_number;
  s32 knob_start_frame;
  float knob_degrees_start;
  float knob_degrees_end;

  bool is_momentary;
  bool is_flashing;
  bool dont_reload_me;

  std::function<void()> generic_paint_func;

};

#endif
