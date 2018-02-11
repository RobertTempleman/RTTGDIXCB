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

s32 RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS = 2;
s32 RTT_EDIT_BOX_BITMAP_BACKGROUND_WIDTH = 16;
s32 RTT_EDIT_BOX_BITMAP_BUTTON_WIDTH     = 15;

rtt_text_edit_box::rtt_text_edit_box(rtt_window&_parent_window,
                                     string _current_text,
                                     string _name,
                                     s32 _x,
                                     s32 _y,
                                     s32 _w,
                                     s32 _h,
                                     vector<string> _entries_to_select,
                                     s32 _current_selected_index,
                                     bool _render_with_window_scroll_offset
                                     ):RTTXCB(_parent_window.wm_member_of.device_context,_w,_h,black_c,_name.data()),
                                       parent_window(_parent_window),
                                       name(_name),
                                       current_text(_current_text),
                                       entries_to_select(_entries_to_select),
                                       current_selected_index(_current_selected_index),
                                       x(_x),
                                       y(_y),
                                       render_with_window_scroll_offset(_render_with_window_scroll_offset),
                                       drop_down_window(unique_ptr<rtt_window>(nullptr)){
  /*drop_down_window(0){*/
  text_bold=true;
  editing_state=RTT_EDIT_BOX_EDITING_STATE_OFF;
  caret_pos=0;
  caret_style=CARET_STYLE_DESELECTED;
  //  text_colour=grey80_c;
  text_colour=white_c;
  already_cached_background=false;
  if (current_selected_index==-1){
    for(u32 i=0;i<entries_to_select.size();i++){
      if (current_text==entries_to_select[i]){
        current_selected_index=i;
        break;
      }
    }
  }
  paint();
}


void rtt_text_edit_box::delete_old_dropdown_selection_window(){
  s32 i=0;
  auto &window_stack=parent_window.wm_member_of.window_stack;
  while(i<window_stack.size()){
    rtt_window &w=*window_stack[i];
    if (w.window_type==WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST){
      window_stack.erase(window_stack.begin()+i);
    }
    i++;
  }
}


void rtt_text_edit_box::open_dropdown_selection_window(){
  u32 n=entries_to_select.size();
  if (n){
    window_manager &lwm=parent_window.wm_member_of;
    s32 client_scrollable_height=n*COMBO_BOX_ITEM_SPACING;
    s32 nw_x=parent_window.x+x+lwm.window_border_width;
    s32 nw_y=parent_window.y-y-lwm.window_title_bar_height-h;
    s32 nw_h=client_scrollable_height+lwm.window_border_width+2;
    s32 parent_window_space_left=parent_window.h-lwm.window_title_bar_height-lwm.window_border_width;
    if (nw_h>parent_window_space_left){
      nw_h=parent_window_space_left;
    }
    delete_old_dropdown_selection_window();
    //    COLORREF wcol=darkseagreen4_c;
    COLORREF wcol=black_c;
    drop_down_window=unique_ptr<rtt_window>(new rtt_window(lwm,65536+parent_window.id,"editboxlist",nw_x,nw_y,w,nw_h,wcol,0,0,WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST,w,client_scrollable_height));
    drop_down_window->text_edit_box_spawned_from=this;
    u32 index=0;
    for(auto it=entries_to_select.begin();it!=entries_to_select.end();it++){
      drop_down_window->new_item(index++, *it);
    }
    drop_down_window->paint();
    lwm.window_stack.push_back(move(drop_down_window));  // stick into window manager window stack as the topmost
    lwm.render_window(lwm.window_stack.size()-1); // and draw it
  }
}


void stretch_horizontal_border(RTTXCB *bitmap_target, RTTXCB *bitmap_source, s32 border_thickness, s32 y_offset){
  // stretch the border outline from a bordered source bitmap to an arbitrary width target bitmap
  s32 sw=bitmap_source->w;
  s32 tw=bitmap_target->w;
  s32 blit_width=sw-border_thickness;
  s32 x=0;
  bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,y_offset,  blit_width,  border_thickness, 0 ,y_offset,false);// copy first span, assume all targets are wider than the source
  tw-=blit_width;
  x+=blit_width;
  blit_width-=border_thickness;
  while(tw>0){
    if (tw<=blit_width){
      // last span so include end corner
      s32 source_x_start=sw-tw;
      bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,y_offset,  tw        ,  border_thickness, source_x_start   ,y_offset,false);
      blit_width=tw;
    }else{
      // copy another border span
      bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,y_offset,  blit_width,  border_thickness, border_thickness ,y_offset,false);
    }
    tw-=blit_width;
    x+=blit_width;
  }
}


void horizontally_tile_bitmap_with_border_to_arbitrary_width_bitmap(RTTXCB *bitmap_target,
                                                                    RTTXCB *bitmap_source,
                                                                    s32 border_thickness,
                                                                    s32 source_bitmap_width,
                                                                    s32 destination_width_to_tile){
  // copy border from source & then tile the specified width of target with bitmap immediately after the border in the source 
  //  s32 sw=bitmap_source->w;
  s32 tw=destination_width_to_tile;
  s32 h=bitmap_target->h-2*border_thickness;
  s32 blit_width=source_bitmap_width+border_thickness;
  s32 x=0;
  bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,border_thickness,  blit_width,  h, 0 ,border_thickness,false);// copy first span, assume all targets are wider than the source
  tw-=blit_width;
  x+=blit_width;
  blit_width=source_bitmap_width;
  while(tw>0){
    if (tw<=blit_width){
      // last span so include end corner
      //      s32 source_x_start=border_thickness+source_bitmap_width-tw;
      bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,border_thickness,  tw        ,  h, border_thickness ,border_thickness,false);
      blit_width=tw;
    }else{
      // copy another border span
      bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,border_thickness,  blit_width,  h, border_thickness ,border_thickness,false);
    }
    tw-=blit_width;
    x+=blit_width;
  }
}


void copy_end_border_and_bitmap(RTTXCB *bitmap_target,
                     RTTXCB *bitmap_source,
                     s32 border_thickness,
                     s32 source_bitmap_width){
  // copy end vertical border from source & if source_bitmap_width>0 copy a bitmap adjacent to the right border too 
  //  s32 sw=bitmap_source->w;
  s32 tw=bitmap_target->w;
  s32 h=bitmap_target->h-2*border_thickness;
  s32 blit_width=source_bitmap_width+border_thickness;
  s32 x=tw-blit_width;
  s32 sx=bitmap_source->w-blit_width;
  bitmap_target->blit_bitmap_to_self(bitmap_source, x  ,border_thickness,  blit_width,  h, sx ,border_thickness,false);// copy first span, assume all targets are wider than the source
}


#define RTT_EDIT_BOX_FONT_START_X RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS+1
#define RTT_EDIT_BOX_FONT_START_Y RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS


s32 rtt_text_edit_box::get_available_text_rendering_width(){
  s32 font_availble_width=w-RTT_EDIT_BOX_FONT_START_X-RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS-1;
  if (entries_to_select.size()){
    font_availble_width-=RTT_EDIT_BOX_BITMAP_BUTTON_WIDTH;
  }
  return font_availble_width;
}


rtt_font& rtt_text_edit_box::get_font(){
  if (text_bold){
    return tahoma8_bold;
  }
  return tahoma8_normal;
}
//rtt_font* rtt_text_edit_box::get_font(){
//  rtt_font *font;
//  if (text_bold){
//    font=&tahoma8_bold;
//  }else{
//    font=&tahoma8_normal;
//  }
//  return font;
//}


void rtt_text_edit_box::paint(){
  printf("text edit box repainted, name=%s\n",name.data());
  s32 font_availble_width=get_available_text_rendering_width();
  if (already_cached_background){
    // background bitmap was already created so just blit it
    blit_bitmap_to_self(cached_background,0,0,w,h,0,0,false);
  }else{
    // create background bitmap by adaptively scaling the editbox template bitmap
    RTTXCB *bm=parent_window.wm_member_of.bitmaps[parent_window.wm_member_of.bitmap_text_box];
    //  RTTXCB *bm=parent_window.wm_member_of->bitmaps[parent_window.wm_member_of->bitmap_window_title_bar_selected];
    // stretch the wm's bitmap out according to the sizes defined above
    // assume border is border width around the outside and the background is on the left inside the border
    // and the button is one the right inside the border
    stretch_horizontal_border(this, bm, RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS, 0                                      );// screenwise top border
    stretch_horizontal_border(this, bm, RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS, h-RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS );
  
    s32 tile_w=w-RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS;
    if (entries_to_select.size()){
      tile_w+=RTT_EDIT_BOX_BITMAP_BUTTON_WIDTH;
    }
    horizontally_tile_bitmap_with_border_to_arbitrary_width_bitmap(this, bm, RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS,RTT_EDIT_BOX_BITMAP_BACKGROUND_WIDTH,tile_w);  // background
  
    if (entries_to_select.size()){
      copy_end_border_and_bitmap(this,bm,RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS,RTT_EDIT_BOX_BITMAP_BUTTON_WIDTH);  // plot with button
    }else{
      copy_end_border_and_bitmap(this,bm,RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS,0);  // plot with no button
    }
    cached_background=new RTTXCB(*this,RTTXCB_COPY_VERBATIM);
    already_cached_background=true;
  }
  
  rtt_font &font=get_font();
  if (caret_pos>=current_text.size()){
    caret_pos=current_text.size()-1;
  }
  switch(editing_state){
    case RTT_EDIT_BOX_EDITING_STATE_OFF:
      font.p(*this, RTT_EDIT_BOX_FONT_START_X, font_availble_width, h-RTT_EDIT_BOX_FONT_START_Y, text_colour, current_text);
      break;
    case RTT_EDIT_BOX_EDITING_STATE_SHOW_CARET_GREYED:
      break;
    case RTT_EDIT_BOX_EDITING_STATE_EDITING:{
      vector<s32> x_coords;
      font.print_save_posns_and_draw_caret(*this, RTT_EDIT_BOX_FONT_START_X, font_availble_width, h-RTT_EDIT_BOX_FONT_START_Y, x_coords, caret_pos, -1,caret_style, text_colour, current_text.data());
      };break;
  }
}


void rtt_text_edit_box::redraw_text_and_caret(vector<s32> &x_coords,bool caret_style){
  s32 font_availble_width=get_available_text_rendering_width();
  get_font().print_save_posns_and_draw_caret(*this, RTT_EDIT_BOX_FONT_START_X, font_availble_width, h-RTT_EDIT_BOX_FONT_START_Y, x_coords, caret_pos, -1,caret_style, text_colour, current_text.data());
}


s32 rtt_text_edit_box::mouse_click(s32 butt,s32 mx,s32 my){
  // mouse coords are window coords
  if (butt==MOUSE_LEFT_DOWN){
    if (my>=y && my<y+h){
      if (mx>=x && mx<x+w){
        s32 button_x_start=x+w-RTT_EDIT_BOX_BITMAP_BORDER_THICKNESS-RTT_EDIT_BOX_BITMAP_BUTTON_WIDTH;
        if (entries_to_select.size()){
          // have button
          if (mx>=button_x_start){
            // open drop down window to select an item;
            open_dropdown_selection_window();
            return RTT_EDIT_BOX_MOUSE_CLICK_RETURN_CLICKED_BUTTON;
          }
        }
        if (mx<button_x_start){
          // clicked inside text area, so detemrine the caret position & set editing state
          editing_state=RTT_EDIT_BOX_EDITING_STATE_EDITING;
          vector<s32> x_coords;
          caret_pos=-1;
          redraw_text_and_caret(x_coords,CARET_STYLE_SOLID_1);// get character start x coords
          caret_pos=0;
          s32 mxt=mx-RTT_EDIT_BOX_FONT_START_X-x;
          if (x_coords.size()){
            for(u32 i=0;i<x_coords.size()-1;i++){
              if (mxt>=x_coords[i] && mxt<x_coords[i+1]){
                caret_pos=(s32)i;
                printf("clicked in edit box %s at character position %d\n",name.data(),caret_pos);
              }
            }
          }
          return RTT_EDIT_BOX_MOUSE_CLICK_RETURN_CLICKED_IN_TEXT_EDIT_BOX_TEXT_AREA;
        }
      }
    }
  }
  return RTT_EDIT_BOX_MOUSE_CLICK_RETURN_NOTHING;
}


u32 rtt_text_edit_box::key(u32 key_action,u32 OS_keycode,bool is_ascii,u8 ASCII){
  // false return means to remove focus from this edit box
  if (key_action==KEYPRESS_DOWN){
    s32 len=(s32)current_text.size();
    if (is_ascii){
      current_text.insert(current_text.begin()+caret_pos,1,ASCII);
      caret_pos++;
      return RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT;
    }else{
      switch(OS_keycode) {
        case RTT_XK_BackSpace:
          if (len && caret_pos>0){
            caret_pos--;
            current_text.erase(current_text.begin()+caret_pos);
          }
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT;
        case RTT_XK_Delete:
          if (len && caret_pos<len){
            current_text.erase(current_text.begin()+caret_pos);
            if (caret_pos>(s32)current_text.size()){
              caret_pos=current_text.size();
            }
          }
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT;
        case RTT_XK_Up:
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_PREVIOUS_EDITBOX;
        case RTT_XK_Left:
          caret_pos--;
          if (caret_pos<0){
            caret_pos=0;
          }
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT;
        case RTT_XK_Right:
          caret_pos++;
          if (caret_pos>=len){
            caret_pos=len-1;
          }
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT;
        case RTT_XK_Down:
        case RTT_XK_Tab:
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_NEXT_EDITBOX;
        case RTT_XK_Return:
          return RTT_TEXT_EDIT_BOX_RETURN_ACTION_FINISHED;
      }
    }
  }
  return RTT_TEXT_EDIT_BOX_RETURN_ACTION_NOTHING;
}


