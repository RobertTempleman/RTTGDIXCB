#include <stdarg.h>
#include "RTTGDIXCB.h"
#include "OS_interface.h"
#include "main.h"
#include "RTT_2.h"
#include "RTT_colours.h"
#include	<cassert>
#include <algorithm>
#include <memory>
#include "RTT_keysymdef.h"
#include "fonts/font_tahoma_8pt.h"
#include "RTT_keysyms.h"

#include "RTT_text_edit_box.h"
#include "RTT_button.h"

s32 RTT_BUTTON_BITMAP_BORDER_THICKNESS = 2;
s32 RTT_BUTTON_BITMAP_BACKGROUND_WIDTH = 16;
s32 RTT_BUTTON_BITMAP_BUTTON_WIDTH     = 15;

rtt_button::rtt_button(rtt_window&_parent_window,
                       string _current_text,
                       string _name,
                       std::function<void(RTTXCB&)> &&click_func,
                       s32 _x,
                       s32 _y,
                       s32 _w,
                       s32 _h,
                       bool _render_with_window_scroll_offset
                       ):RTTXCB(_parent_window.wm_member_of.device_context,_w,_h,black_c,_name.data()),
                         parent_window(_parent_window),
                         current_text(_current_text),
                         name(_name),
                         click_function(std::move(click_func)),
                         x(_x),
                         y(_y),
                         render_with_window_scroll_offset(_render_with_window_scroll_offset){
  text_bold=true;
  //  text_bold=false;
  text_colour=white_c;
  already_cached_background=false;
  pressed=false;
  paint();
}

#define RTT_BUTTON_FONT_START_X RTT_BUTTON_BITMAP_BORDER_THICKNESS+3
#define RTT_BUTTON_FONT_START_Y RTT_BUTTON_BITMAP_BORDER_THICKNESS-1

s32 rtt_button::get_available_text_rendering_width(){
  s32 font_availble_width=w-RTT_BUTTON_FONT_START_X-RTT_BUTTON_BITMAP_BORDER_THICKNESS-1;
  return font_availble_width;
}


rtt_font& rtt_button::get_font(){
  if (text_bold){
    return tahoma8_bold;
  }
  return tahoma8_normal;
}


void rtt_button::make_cached_bitmap(RTTXCB **rttxcb,u32 bitmap){
  // create background bitmap by adaptively scaling the editbox template bitmap
  RTTXCB *bm=parent_window.wm_member_of.bitmaps[bitmap];
  // stretch the wm's bitmap out according to the sizes defined above
  // assume border is border width around the outside and the background is on the left inside the border
  // and the button is one the right inside the border
  stretch_horizontal_border(this, bm, RTT_BUTTON_BITMAP_BORDER_THICKNESS, 0                                      );// screenwise top border
  stretch_horizontal_border(this, bm, RTT_BUTTON_BITMAP_BORDER_THICKNESS, h-RTT_BUTTON_BITMAP_BORDER_THICKNESS );
  
  s32 tile_w=w-RTT_BUTTON_BITMAP_BORDER_THICKNESS;
  horizontally_tile_bitmap_with_border_to_arbitrary_width_bitmap(this, bm, RTT_BUTTON_BITMAP_BORDER_THICKNESS,RTT_BUTTON_BITMAP_BACKGROUND_WIDTH,tile_w);  // background
  copy_end_border_and_bitmap(this,bm,RTT_BUTTON_BITMAP_BORDER_THICKNESS,0);  // plot with no button
  *rttxcb=new RTTXCB(*this,RTTXCB_COPY_VERBATIM);
}

#define RTT_BUTTON_PRESSED_FONT_DELTA_X -1 // when pressed the font is offset by this much
#define RTT_BUTTON_PRESSED_FONT_DELTA_Y 1  // when pressed the font is offset by this much

void rtt_button::paint(){
  printf("button repainted, name=%s\n",name.data());
  s32 font_availble_width=get_available_text_rendering_width();
  s32 font_offset_x=0;
  s32 font_offset_y=0;
  if (already_cached_background){
    // background bitmap was already created so just blit it
    if (pressed){
      font_offset_x=RTT_BUTTON_PRESSED_FONT_DELTA_X;
      font_offset_y=RTT_BUTTON_PRESSED_FONT_DELTA_Y;
      blit_bitmap_to_self(cached_background_pressed,0,0,w,h,0,0,false);
    }else{
      blit_bitmap_to_self(cached_background_normal,0,0,w,h,0,0,false);
    }
  }else{
    make_cached_bitmap(&cached_background_normal, parent_window.wm_member_of.bitmap_button_normal);
    make_cached_bitmap(&cached_background_pressed, parent_window.wm_member_of.bitmap_button_pressed);
    already_cached_background=true;
  }
  get_font().p(*this, RTT_BUTTON_FONT_START_X+font_offset_x, font_availble_width, h-RTT_BUTTON_FONT_START_Y+font_offset_y, text_colour, current_text);
}


s32 rtt_button::mouse_click(s32 butt,s32 mx,s32 my){
  // mouse coords are window coords
  switch(butt){
    case MOUSE_LEFT_DOWN:
      if (my>=y && my<y+h){
        if (mx>=x && mx<x+w){
          pressed=true;
          printf("clicked on button %s\n",name.data());
          click_function(*this);
          return RTT_BUTTON_MOUSE_CLICKED_BUTTON;
        }
      }
      break;
    case MOUSE_LEFT_UP:
      if (pressed){
        printf("released button %s\n",name.data());
        pressed=false;
        return RTT_BUTTON_MOUSE_UNCLICKED_BUTTON;
      }
      break;
  }
  return RTT_BUTTON_MOUSE_RETURN_NOTHING;
}


u32 rtt_button::key(u32 key_action,u32 OS_keycode,bool is_ascii,u8 ASCII){
  return RTT_TEXT_EDIT_BOX_RETURN_ACTION_NOTHING;
}


