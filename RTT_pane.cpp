#include <stdarg.h>
#include "RTTGDIXCB.h"
#include "OS_interface.h"
#include "main.h"
#include "RTT_2.h"
#include "RTT_colours.h"
#include <cassert>
#include <algorithm>
#include <memory>
#include "RTT_keysymdef.h"
#include "fonts/font_tahoma_8pt.h"
#include "RTT_keysyms.h"

#include "RTT_text_edit_box.h"
#include "RTT_pane.h"

s32 RTT_PANE_BORDER_THICKNESS = 2;
s32 RTT_PANE_BACKGROUND_WIDTH = 16;
s32 RTT_PANE_BUTTON_WIDTH     = 15;

//static const bool mouse_wheel_moves_everything_for_fun=true;
static const bool mouse_wheel_moves_everything_for_fun=false;

rtt_pane::rtt_pane(rtt_window&_parent_window,
                   string _name,
                   paintfn_type &&_paint_fn,
                   clickfn_type &&_click_fn,
                   mouse_move_fn_type &&_mouse_move_fn,
                   s32 _x,
                   s32 _y,
                   s32 _w,
                   s32 _h,
                   bool _render_with_window_scroll_offset
                   ):RTTXCB(_parent_window.wm_member_of.device_context,_w,_h,black_c,_name.data()),
                     parent_window(_parent_window),
                     name(_name),
                     x(_x),
                     y(_y),
                     render_with_window_scroll_offset(_render_with_window_scroll_offset),
                     paint_fn(std::move(_paint_fn)),
                     click_fn(std::move(_click_fn)),
                     mouse_move_fn(std::move(_mouse_move_fn)),
                     mouse_was_clicked_inside_pane(false),
                     do_a_refresh(false),
                     pane_special_functionality(PANE_SPECIAL_FUNCTION_NOTHING),
                     knob_frame_number(0),
                     knob_degrees_start(0),
                     knob_degrees_end(0),
                     knob_start_frame(0){
  paint();
}


rtt_font& rtt_pane::get_font(){
  if (text_bold){
    return tahoma8_bold;
  }
  return tahoma8_normal;
}


void rtt_pane::paint(){
  printf("  pane repainted, name=%s\n",name.data());
  switch(pane_special_functionality){
    case PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_SWITCH:
      if (extra_bitmap){
        RTTXCB &knob=*extra_bitmap;
        s32 knob_h=knob.h/SWITCH_KNOB_BITMAP_NUM_FRAMES;
        blit_bitmap_to_self(knob,0,0,knob.w,knob_h, 0, knob_h*knob_frame_number,false);
      }
      break;
    case PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL:
      if (extra_bitmap){
        RTTXCB &knob=*extra_bitmap;
        blit_bitmap_to_self(knob,0,0,knob.w,knob.w, 0, knob.w*knob_frame_number,false);
      }
      break;
    case PANE_SPECIAL_FUNCTION_PAINT_FUNCTION_IS_GENERIC_STD_FUNCTION_TO_CALL:
      generic_paint_func();
      break;
  }
  if (paint_fn){
    paint_fn(*this);
  }
  do_a_refresh=false;
  //  blit_bitmap_to_self(cached_background_pressed,0,0,w,h,0,0,false);
  //  get_font().p(*this, RTT_BUTTON_FONT_START_X+font_offset_x, font_availble_width, h-RTT_BUTTON_FONT_START_Y+font_offset_y, text_colour, current_text);
}


bool rtt_pane::handle_knob_mouse_wheel(s32 increment){
  //  s32 pane_centre_x=w/2;
  //  s32 pane_centre_y=h/2;
  //  float mouse_dx=(float)(pane_mouse_x-pane_centre_x);
  //  float mouse_dy=(float)(pane_mouse_y-pane_centre_y);
  //  float mouse_ang=atan2f(-mouse_dx,mouse_dy);
  //  if (mouse_ang<0){
  //    mouse_ang=mouse_ang+RTT_2PI;
  //  }
  //  float ang_range=knob_degrees_end-knob_degrees_start;
  //  float mouse_knob_ang_offset=mouse_ang*RTT_RADS_TO_DEGREES-knob_degrees_start;
  RTTXCB &knob=*extra_bitmap;
  s32 num_frames=knob.h/knob.w;
  //  s32 knob_frame_number_test=(s32)((float)(num_frames-1)*mouse_knob_ang_offset/ang_range+0.5f);
  s32 knob_frame_number_test=knob_frame_number+increment;
  if (knob_frame_number_test>=0 && knob_frame_number_test<num_frames){
    knob_frame_number=knob_frame_number_test;
    return true;
  }
  return false;
}


bool rtt_pane::handle_knob_dial_display_frame_from_mouse_pos(s32 pane_mouse_x,s32 pane_mouse_y){
  s32 pane_centre_x=w/2;
  s32 pane_centre_y=h/2;
  float mouse_dx=(float)(pane_mouse_x-pane_centre_x);
  float mouse_dy=(float)(pane_mouse_y-pane_centre_y);
  float mouse_ang=atan2f(-mouse_dx,mouse_dy);
  if (mouse_ang<0){
    mouse_ang=mouse_ang+RTT_2PI;
  }
  float ang_range=knob_degrees_end-knob_degrees_start;
  float mouse_knob_ang_offset=mouse_ang*RTT_RADS_TO_DEGREES-knob_degrees_start;
  RTTXCB &knob=*extra_bitmap;
  s32 num_frames=knob.h/knob.w;
  s32 knob_frame_number_test=(s32)((float)(num_frames-1)*mouse_knob_ang_offset/ang_range+0.5f);
  if (knob_frame_number_test>=0 && knob_frame_number_test<num_frames){
    knob_frame_number=knob_frame_number_test;
    knob_float_to_change_to_reflect_current_setting=(float)knob_frame_number_test/(float)(num_frames-1);
    assert(knob_float_to_change_to_reflect_current_setting>=0 && knob_float_to_change_to_reflect_current_setting<=1.0f);
    return true;
  }
  return false;
}


bool rtt_pane::mouse_move(s32 mx,s32 my){
  bool inside_pane=false;
  s32 pane_mouse_x=mx-x;
  s32 pane_mouse_y=my-y;
  switch(pane_special_functionality){
    case PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL:
      if (mouse_was_clicked_inside_pane){
        do_a_refresh=handle_knob_dial_display_frame_from_mouse_pos(pane_mouse_x,pane_mouse_y);
        if (do_a_refresh){
          RTTXCB &knob=*extra_bitmap;
          blit_bitmap_to_self(knob,0,0,knob.w,knob.w, 0, 0,false);
        }
      }
      break;
  }
  if (my>=y && my<y+h){
    if (mx>=x && mx<x+w){
      //      printf("clicked inside pane %s\n",name.data());
      //      mouse_was_clicked_inside_pane=true;
      inside_pane=true;
      if (mouse_move_fn){
        mouse_move_fn(*this,mx-x,my-y);
      }
      //      return RTT_BUTTON_MOUSE_CLICKED_BUTTON;
    }
  }
  
  return do_a_refresh;
}


s32 rtt_pane::mouse_click(s32 butt,s32 mx,s32 my){
  // mouse coords are window coords
  switch(butt){
    case MOUSE_LEFT_DOWN:
      if (my>=y && my<y+h){
        if (mx>=x && mx<x+w){
          printf("clicked inside pane %s\n",name.data());
          mouse_was_clicked_inside_pane=true;
          if (click_fn){
            click_fn(*this,butt,mx-x,my-y);
          }
          s32 pane_mouse_x=mx-x;
          s32 pane_mouse_y=my-y;
          switch(pane_special_functionality){
            case PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL:
              do_a_refresh=handle_knob_dial_display_frame_from_mouse_pos(pane_mouse_x,pane_mouse_y);
              //              if (do_a_refresh){
              //                RTTXCB &knob=*extra_bitmap;
              //                blit_bitmap_to_self(knob,0,0,knob.w,knob.w, 0, 0,false);
              //              }
              break;
            case PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_SWITCH:
              if (is_momentary){
                knob_frame_number=1;
                knob_float_to_change_to_reflect_current_setting=1.0f;
              }else{
                knob_frame_number^=1;
              }
              do_a_refresh=true;
              break;
            case PANE_SPECIAL_FUNCTION_PAINT_FUNCTION_IS_GENERIC_STD_FUNCTION_TO_CALL:
              do_a_refresh=true;
              break;
          }
          return RTT_BUTTON_MOUSE_CLICKED_BUTTON;
        }
      }
      break;
    case MOUSE_LEFT_UP:
      if (mouse_was_clicked_inside_pane){
        mouse_was_clicked_inside_pane=false;
        printf("mouse click originating inside pane %s was released\n",name.data());
        if (click_fn){
          click_fn(*this,butt,mx-x,my-y);
        }
        return RTT_BUTTON_MOUSE_UNCLICKED_BUTTON;
      }
      break;
    case MOUSE_WHEEL_DOWN:
    case MOUSE_WHEEL_UP:
      if ((my>=y && my<y+h)||mouse_wheel_moves_everything_for_fun){
        if ((mx>=x && mx<x+w)||mouse_wheel_moves_everything_for_fun){
          switch(pane_special_functionality){
            case PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL:
              do_a_refresh=handle_knob_mouse_wheel(butt==MOUSE_WHEEL_UP?1:-1);
              //              if (do_a_refresh){
              //                RTTXCB &knob=*extra_bitmap;
              //                blit_bitmap_to_self(knob,0,0,knob.w,knob.w, 0, 0,false);
              //              }
              break;
          }
        }
      }
      break;
  }
  return RTT_BUTTON_MOUSE_RETURN_NOTHING;
}


u32 rtt_pane::key(u32 key_action,u32 OS_keycode,bool is_ascii,u8 ASCII){
  return RTT_TEXT_EDIT_BOX_RETURN_ACTION_NOTHING;
}


// convert pane into a dial like control using a knobman like control bitmap
void rtt_pane::add_dial(string name,s32 _x,s32 _y,s32 _start_frame,float _degrees_start, float _degrees_end,string _group,string _desc,bool _dont_reload_me){
  knob_degrees_start=_degrees_start;
  knob_degrees_end=_degrees_end;
  knob_start_frame=_start_frame;
  dont_reload_me=_dont_reload_me;
  pane_special_functionality=PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL;
  extra_bitmap=unique_ptr<RTTXCB>(new RTTXCB(parent_window.wm_member_of.device_context,name.data()));
  paint();
}


// convert pane into a dial like control using a knobman like control bitmap
void rtt_pane::add_dial(unique_ptr<RTTXCB> &knob_bitmap,s32 _x,s32 _y,s32 _start_frame,float _degrees_start, float _degrees_end,string _group,string _desc,bool _dont_reload_me){
  knob_degrees_start=_degrees_start;
  knob_degrees_end=_degrees_end;
  knob_start_frame=_start_frame;
  dont_reload_me=_dont_reload_me;
  pane_special_functionality=PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_DIAL;
  extra_bitmap=move(knob_bitmap);
  paint();
}


// convert pane into a dial like control using a knobman like control bitmap
void rtt_pane::add_switch(unique_ptr<RTTXCB> &knob_bitmap,
                          s32 _x,
                          s32 _y,
                          string _group,
                          string _desc,
                          bool _is_momentary,
                          bool _is_flashing,
                          bool _dont_reload_me){
  knob_frame_number=0;
  is_momentary=_is_momentary;
  is_flashing=_is_flashing;
  dont_reload_me=_dont_reload_me;
  pane_special_functionality=PANE_SPECIAL_FUNCTION_IS_A_KNOBMAN_STYLE_SWITCH;
  extra_bitmap=move(knob_bitmap);
  paint();
}
