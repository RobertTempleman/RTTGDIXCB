// contains some OS specifics to interface application with whatever renderer backend is used
// handles multiple window interface functionality, so that application only has to care about individual window geometry

#include <stdarg.h>
#include "RTTGDIXCB.h"
#include "OS_interface.h"
#include "main.h"
#include "RTT_2.h"
#include "RTT_colours.h"
#include	<cassert>
#include <algorithm>
#include "fonts/font_tahoma_8pt.h"
//#include "RTT_Window.h"

#define DEBUG_SEE_CALLS 0

int rtt_window::sort_index=0;

#define TASK_BAR_WIDTH 200
#define TASK_BAR_OFFSET_FROM_TOP_OF_SCREEN_TO_START_OF_WINDOW_ITEMS 200

rtt_window::~rtt_window(){
  if (SEE_GRAPHICS_CREATION_DEBUG_LOGGING){
    printf("  destructing rtt_window %s this=%16x\n",title.data(),(uint64_t)this);
  }
}


rtt_window::rtt_window(window_manager &_wm_member_of,
                       int _id,
                       const char*_title,
                       int _x,
                       int _y,
                       int _w,
                       int _h,
                       COLORREF _window_background_col,
                       win_cb _init_fn,
                       win_cb _paint_fn,
                       int _window_type,
                       int _client_scrollable_width,
                       int _client_scrollable_height):wm_member_of(_wm_member_of),
                                                      RTTXCB(_wm_member_of.device_context, _w, _h, _window_background_col, _title),
                                                      init_fn(_init_fn),
                                                      paint_fn(_paint_fn),
                                                      window_type(_window_type),
                                                      is_selected(false),
                                                      last_is_selected(false),
                                                      text_edit_box_spawned_from(0),
                                                      last_pane_index(-1){
  
  //  default_font_col_multiple_sel
  id=_id;
  col_combo_box_single_selection_font              =darkred_c;
  col_combo_box_single_selection_rect_background   =black_c;
  col_combo_box_single_selection_rect_border       =white_c;
  
  col_combo_box_multiple_selection_rect_background =grey54_c;
  col_combo_box_multiple_selection_rect_border     =grey54_c;
  col_combo_box_multiple_selection_font            =electricultramarine_c;//darkblue_c;
  default_font_col                                 =lightblue_c;
  selected_item=-1;
  selected_item_alt=-1;
  horizontal_display_offset=0;
  vertical_display_offset=0;
  window_display_width_pixels=0;
  window_display_height_pixels=0;
  scrollbar_h.display_offset_start=0;
  scrollbar_h.client_total_length_pixels=_client_scrollable_width;
  scrollbar_v.display_offset_start=0;
  scrollbar_v.client_total_length_pixels=_client_scrollable_height;
  if (_title){
    title=_title;
  }
  x=_x;
  y=_y;
  is_minimized=false;
  is_maximized=false;
  my_sort_key=sort_index++;
  last_title_bar_width_deselected=0;
  last_title_bar_width_selected=0;
  title_bar_selected=0;
  title_bar_deselected=0;
  currently_selected_text_edit_box=-1;
  draw_furniture();
  grabbed_h_scrollbar=false;
  grabbed_v_scrollbar=false;
  //  paint_controls();
  paint();
}


bool rtt_window::operator<(const rtt_window&w){
  return my_sort_key<w.my_sort_key;
}


bool rtt_window::check_inside_title_bar(pt &p){
  if (p.x>x && p.x<x+w){
    if (p.y<=y && p.y>y-window_manager::window_title_bar_height ){
      return true;
    }
  }
  return false;
}


int rtt_window::check_inside_buttons(pt &p){
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
    case WINDOW_DISPLAY_TYPE_PANE_WINDOW:
      {
        RTTXCB *wb=wm.bitmaps[window_manager::bitmap_window_buttons_selected];
        int bxe=x+w-WINDOW_BUTTON_X_OFFSET;
        int bxs=bxe-wb->w;
        if (p.x>=bxs && p.x<bxs+WINDOW_BUTTON_WIDTH){
          return WINDOW_BUTTON_MINIMIZE;
        }else if (p.x>=bxs+WINDOW_BUTTON_WIDTH && p.x<bxs+2*WINDOW_BUTTON_WIDTH){
          return WINDOW_BUTTON_MAXIMIZE;
        }else if (p.x>=bxe-WINDOW_BUTTON_WIDTH && p.x<bxe){
          return WINDOW_BUTTON_CLOSE;
        }
      }
      break;
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      {
        int bxe=x+w-WINDOW_BUTTON_X_OFFSET;
        if (p.x>=bxe-WINDOW_BUTTON_WIDTH && p.x<bxe){
          return WINDOW_BUTTON_CLOSE;
        }
      }
      break;
  }
  return 0;
}


#define EBW EXTRA_WINDOW_BORDER_SIZE_FOR_GRABBING_RESIZE_BORDERS_EASILY
bool rtt_window::check_inside_right_border(pt &p){
  if (p.x>=x+w-window_manager::window_border_width && p.x<x+w+EBW){
    if (p.y>y-h-EBW && p.y<=y+EBW ){
      return true;
    }
  }
  return false;
}


bool rtt_window::check_inside_left_border(pt &p){
  if (p.x>=x-EBW && p.x<x+window_manager::window_border_width){
    if (p.y>y-h-EBW && p.y<=y+EBW ){
      return true;
    }
  }
  return false;
}


bool rtt_window::check_inside_bottom_border(pt &p){
  if (p.x>=x-EBW && p.x<x+w+EBW){
    if (p.y>y-h-EBW && p.y<=y-h+window_manager::window_border_width){
      return true;
    }
  }
  return false;
}


bool rtt_window::check_inside_top_border(pt &p){
  if (p.x>x-EBW && p.x<x+w+EBW){
    if (p.y<=y+EBW && p.y>y-window_manager::window_border_width){
      return true;
    }
  }
  return false;
}


bool rtt_window::check_inside_window(pt &p){
  if (p.x>x+window_manager::window_border_width && p.x<x+w-window_manager::window_border_width){
    if (p.y<=y-window_manager::window_title_bar_height && p.y>y-h+window_manager::window_border_width){
      return true;
    }
  }
  return false;
}


#define COMBO_BOX_FONT_PLOTTING_SELECTION_VERTICAL_OFFSET 3
bool rtt_window::check_combo_mouse_posn(int xt,int yt,u32 mouse_button){
  // this accepts the window manager relative mouse coordinates
  bool title_bar_present=false;
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      title_bar_present=true;
      break;
    case WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST:
    case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST:
      title_bar_present=false;
      break;
  }
  int n=(int)items.size();
  if (!n){
    selected_item=-1;
    selected_item_alt=-1;
    return false;
  }
  bool needs_a_redraw=false;
  int wbw=wm_member_of.window_border_width;
  int wbw2=wbw>>1;
  int wtbw;
  if (title_bar_present){
    wtbw=wm_member_of.window_title_bar_height;
  }else{
    wtbw=wbw2;
  }
  int cw=w-wbw-DEFAULT_SCROLL_BAR_WIDTH;
  int ytop=y-wtbw-COMBO_BOX_FONT_PLOTTING_SELECTION_VERTICAL_OFFSET;
  if (xt>x+wbw2 && xt<x+cw){
    if (yt>y-h+wbw2 && yt<ytop){
      int scroll_offet_start_index=vertical_display_offset/COMBO_BOX_ITEM_SPACING;
      int visible_index=(ytop-yt)/COMBO_BOX_ITEM_SPACING;
      int new_selected_item=scroll_offet_start_index+visible_index;
      switch(window_type){
        case WINDOW_DISPLAY_TYPE_COMBO_BOX:
        case WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST:
          if (new_selected_item!=selected_item){
            needs_a_redraw=true;
          }
          selected_item=new_selected_item;
          assert(selected_item>=0);
          if (selected_item>=n){
            selected_item=n-1;
          }
          title=items[selected_item].text;
          break;
        case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST:
          // allow multiple selection, control toggles, shift allows block select, left button selects, right button deselects
          if (wm_member_of.key_shift_pressed){
            if (selected_item_alt==-1){
              selected_item_alt=new_selected_item;
              switch(mouse_button){
                case MOUSE_LEFT_DOWN:{
                  auto res=find(selected_items.begin(),selected_items.end(),new_selected_item);
                  if (res==selected_items.end()){
                    selected_items.push_back(new_selected_item);
                    needs_a_redraw=true;
                  }
                  break;
                }
                case MOUSE_RIGHT_DOWN:{
                  auto res=find(selected_items.begin(),selected_items.end(),new_selected_item);
                  if (res!=selected_items.end()){
                    selected_items.erase(res);
                    needs_a_redraw=true;
                  }
                  break;
                }
              }
            }else{
              u32 st;
              u32 en;
              if (selected_item_alt>new_selected_item){
                st=new_selected_item;
                en=selected_item_alt;
              }else{
                st=selected_item_alt;
                en=new_selected_item;
              }
              switch(mouse_button){
                case MOUSE_LEFT_DOWN:{
                  for(u32 i=st;i<=en;i++){
                    auto res=find(selected_items.begin(),selected_items.end(),i);  
                    if (res==selected_items.end()){
                      selected_items.push_back(i);
                    }
                  }
                  break;
                }
                case MOUSE_RIGHT_DOWN:
                  for(u32 i=st;i<=en;i++){
                    auto res=find(selected_items.begin(),selected_items.end(),i);  
                    if (res!=selected_items.end()){
                      selected_items.erase(res);
                    }
                  }
                  break;
              }
              selected_item_alt=-1;// reset shift to allow selection of a new block
              needs_a_redraw=true;
            }
          }else if (wm_member_of.key_control_pressed){
            if (new_selected_item<n){
              auto res=find(selected_items.begin(),selected_items.end(),new_selected_item);  
              if (res==selected_items.end()){
                printf("  added a new item to multiple selction list %d\n",new_selected_item);
                selected_items.push_back(new_selected_item);
              }else{
                selected_items.erase(res);
              }
              needs_a_redraw=true;
            }
          }else{
            if (new_selected_item!=selected_item){
              needs_a_redraw=true;
            }
            selected_item=new_selected_item;
            assert(selected_item>=0);
            if (selected_item>=n){
              selected_item=n-1;
            }
            break;
          }
          break;
      }
    }
  }
  return needs_a_redraw;
}


void rtt_window::paint_as_combo_box(bool title_bar_present){
  // displays a scrollable list of mouse-selectable options, optional window title bar
  int wbw=wm_member_of.window_border_width;
  int wbw2=wbw>>1;
  int wtbw;
  if (title_bar_present){
    wtbw=wm_member_of.window_title_bar_height;
  }else{
    wtbw=wbw2;
  }
  clear();
  //  rectangle_fill(0,h,w,wtbw,grey40_c,grey40_c);
  int cw=w-wbw-DEFAULT_SCROLL_BAR_WIDTH-2;
  int n=(int)items.size();
  int scroll_offet_start_index=vertical_display_offset/COMBO_BOX_ITEM_SPACING;
//  int scroll_offsety=scroll_offet_start_index*COMBO_BOX_ITEM_SPACING;
  int yp=h-wtbw;
  for(int i=scroll_offet_start_index;i<n;i++){
    if (i==selected_item){
      rectangle_fill(wbw2,yp-1, cw, COMBO_BOX_ITEM_SPACING, col_combo_box_single_selection_rect_border, col_combo_box_single_selection_rect_background);
      tahoma8_bold.p(*this, wbw, cw, yp, col_combo_box_single_selection_font, items[i].text.data());
    }else if (find(selected_items.begin(), selected_items.end(), i)!=selected_items.end()){
      rectangle_fill(wbw2, yp-1, cw, COMBO_BOX_ITEM_SPACING, col_combo_box_multiple_selection_rect_border, col_combo_box_multiple_selection_rect_background);
      tahoma8_bold.p(*this, wbw, cw, yp, col_combo_box_multiple_selection_font, items[i].text.data());
    }else{
      tahoma8_normal.p(*this, wbw, cw, yp, default_font_col, items[i].text.data());
    }
    yp-=COMBO_BOX_ITEM_SPACING;
  }
  draw_furniture();
}


void rtt_window::rectangle_fill(int rx,int ry,int rw,int rh,COLORREF cr,COLORREF cr_fill){
  // +y is up when using this override
  RTTXCB::rectangle_fill(rx,h-ry,rw,rh,cr,cr_fill);
}


void rtt_window::paint(){
  //  if (break_now){
  //    assert(0);
  //  }
#if DEBUG_SEE_CALLS==1
  printf("  RTT_window::paint() %s\n",name);
  //  if (break_now){
  //    assert(0);
  //  }
#endif
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_PANE_WINDOW:
    case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
      if (paint_fn){
        paint_fn(*this);
        draw_furniture();
        draw_controls();
      }
      break;
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      paint_as_combo_box();
      break;
    case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST:
    case WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST: 
      paint_as_combo_box(false);
      break;
    case WINDOW_DISPLAY_TYPE_BACKGROUND:
      break;  // needs no updating
  }
}


void rtt_window::update_furniture_but_dont_repaint_contents(){
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_PANE_WINDOW:
    case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
      //      if (paint_fn){
      //        paint_fn(this);
      //      }
      draw_furniture();
      break;
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      paint_as_combo_box();
      break;
    case WINDOW_DISPLAY_TYPE_BACKGROUND:
      break;  // needs no updating
  }
}


#define CLIP_DEBUG 0

bool rtt_window::clip_rect_to_window_extents(s32 &rect_x ,s32 &rect_y, s32 &rect_w, s32 &rect_h){
  // true=rect is in windwo extent so needs some rendering
  //  return true;
#if CLIP_DEBUG==1
  printf("\nwindow parms: x y w h %d %d %d %d\n",x,y,w,h);
  printf("0 x y w h %d %d %d %d\n",rect_x,rect_y,rect_w,rect_h);
#endif
  // window top left is its origin so
  s32 window_top_y=y;
  s32 window_bottom_y=y-h;
  if (rect_x>=x+w){
#if CLIP_DEBUG==1
    printf("rect is off the right of window so returning\n");
#endif
    return false;
  }
  if (rect_y>=y){
#if CLIP_DEBUG==1
    printf("rect is off the top of window so returning\n");
#endif
    return false;
  }
  if (rect_x+rect_w<=x){
#if CLIP_DEBUG==1
    printf("rect is off the left of window so returning\n");
#endif
    return false;
  }
  if (rect_y+rect_h<=window_bottom_y){
#if CLIP_DEBUG==1
    printf("rect is off the bottom of window so returning\n");
#endif
    return false;
  }
  // rect_x & rect_y are the upper left coords of the screen with Y assumed increasing up the display
  if (rect_w<=0){
    rect_w=0;
    return false;
  }
  assert(rect_h>0);
  // clip left side
  if (rect_x<x){
#if CLIP_DEBUG==1
    printf("clip left side\n");
#endif
    s32 dx=x-rect_x;
    rect_w-=dx;
    rect_x=x;
    assert(rect_w>0);
  }
#if CLIP_DEBUG==1
  printf("1 x y w h %d %d %d %d\n",rect_x,rect_y,rect_w,rect_h);
#endif
  // clip right side
  if (rect_x+rect_w>x+w){
#if CLIP_DEBUG==1
    printf("clip right side\n");
#endif
    rect_w=x+w-rect_x;
    assert(rect_w>0);
  }
#if CLIP_DEBUG==1
  printf("2 x y w h %d %d %d %d\n",rect_x,rect_y,rect_w,rect_h);
#endif
  // clip bottom
  if (rect_y<window_bottom_y){
#if CLIP_DEBUG==1
    printf("clip bottom\n");
#endif
    s32 dy=window_bottom_y-rect_y;
    rect_h-=dy;
    rect_y=window_bottom_y;
    assert(rect_h>0);
  }
#if CLIP_DEBUG==1
  printf("3 x y w h %d %d %d %d\n",rect_x,rect_y,rect_w,rect_h);
#endif
  // clip top
  s32 rect_top_y=rect_y+rect_h;
  if (rect_top_y>window_top_y){
#if CLIP_DEBUG==1
    printf("clip top\n");
#endif
    s32 dy=rect_top_y-window_top_y;
    rect_h-=dy;
    assert(rect_h>0);
  }
#if CLIP_DEBUG==1
  printf("4 x y w h %d %d %d %d\n\n\n",rect_x,rect_y,rect_w,rect_h);
#endif
  assert(rect_x>=x);
  assert(rect_y>=window_bottom_y);
  assert(rect_w<=w);
  assert(rect_h<=h);
  assert(rect_x+rect_w<=x+w);
  assert(rect_y+rect_h<=window_top_y);
  return true;
}


void rtt_window::render_exposed_screen_rectangle(s32 rect_x ,s32 rect_y, s32 rect_w, s32 rect_h){
  //void RTTXCB::blit_bitmap_to_screen(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y,int main_winow_h){
  // rect_x & rect_y are the upper left coords of the screen with Y assumed increasing up the display
  //  blit_bitmap_to_screen_debug_rect(rect_x, rect_y, rect_w, rect_h, 0,0, wm_member_of.main_h);
  if (clip_rect_to_window_extents(rect_x,rect_y,rect_w,rect_h)){
    blit_bitmap_to_screen_window_mode(rect_x, rect_y, rect_w, rect_h, x, y, wm_member_of.main_h);
  }
}


void rtt_window::remember_size(){
  pre_maximinimize_x=x;
  pre_maximinimize_y=y;
  pre_maximinimize_w=w;
  pre_maximinimize_h=h;
}


void rtt_window::minimize(){
//  pre_maximinimize_x=x;
//  pre_maximinimize_y=y;
//  pre_maximinimize_w=w;
//  pre_maximinimize_h=h;
  x-=-wm_member_of.main_w-1000-w;// minimize just moves off screen
  is_minimized=true;
  is_selected=false;
}


void rtt_window::invalidate(){
  blit_entire_bitmap_to_screen(x,y,wm_member_of.main_h);
}


void rtt_window::maximize(){
  if (is_maximized){
    is_maximized=false;
    un_maximize();
  }else{
    pre_maximinimize_x=x;
    pre_maximinimize_y=y;
    pre_maximinimize_w=w;
    pre_maximinimize_h=h;
    x=0;
    w=wm_member_of.main_w;
    h=wm_member_of.main_h;
    y=h-1;
    is_maximized=true;
  }
}


void rtt_window::un_maximize(){
  x=pre_maximinimize_x;
  y=pre_maximinimize_y;
  w=pre_maximinimize_w;
  h=pre_maximinimize_h;
  is_maximized=false;
}


void rtt_window::un_minimize(){
  x+=-wm_member_of.main_w-1000-w;// minimize just moves off screen
  is_minimized=false;
}


void rtt_window::blit_bitmap_to_workarea_only(RTTXCB *rttgdi_source){
  int screen_x=wm_member_of.window_border_width;
  int screen_y=h-wm_member_of.bitmaps[wm_member_of.bitmap_window_title_bar_selected]->h;
  int screen_w=w-2*wm_member_of.window_border_width;
  int screen_h=h-wm_member_of.bitmaps[wm_member_of.bitmap_window_title_bar_selected]->h-wm_member_of.window_border_width;
  if (scrollbar_h.client_total_length_pixels){
    screen_h-=DEFAULT_SCROLL_BAR_WIDTH;
  }
  if (scrollbar_v.client_total_length_pixels){
    screen_w-=DEFAULT_SCROLL_BAR_WIDTH;
  }
  //  s32 art=visible_workarea_width();
  //  s32 brt=scrollbar_h.client_total_length_pixels;
  s32 max_hor_display_offset=scrollbar_h.client_total_length_pixels-visible_workarea_width();
  if (horizontal_display_offset>max_hor_display_offset){
    horizontal_display_offset=max_hor_display_offset;
  }
  s32 max_ver_display_offset=scrollbar_v.client_total_length_pixels-visible_workarea_height();
  if (vertical_display_offset>max_ver_display_offset){
    vertical_display_offset=max_ver_display_offset;
  }
  blit_bitmap_to_self(rttgdi_source, screen_x, screen_y, screen_w, screen_h, horizontal_display_offset, vertical_display_offset);
}


s32 rtt_window::visible_workarea_width(){
  s32 workarea_width=w-wm_member_of.window_border_width*2;
  if (scrollbar_v.client_total_length_pixels){
    s32 scrollbar_width=wm_member_of.bitmaps[wm_member_of.bitmap_scrollbar_vertical_background]->w;
    workarea_width-=scrollbar_width;
  }
  return workarea_width;
}


s32 rtt_window::visible_workarea_height(){
  s32 workarea_height=h-wm_member_of.window_title_bar_height-wm_member_of.window_border_width;
  if (scrollbar_h.client_total_length_pixels){
    // have scrollbar
    s32 scrollbar_height=wm_member_of.bitmaps[wm_member_of.bitmap_scrollbar_horizontal_background]->h;
    workarea_height-=scrollbar_height;
  }
  return workarea_height;
}


s32 rtt_window::visible_workarea_end_x(){
  return visible_workarea_width()+wm_member_of.window_border_width;
}


s32 rtt_window::visible_workarea_end_y(){
  // y distance in y-down screen from top of window to top of bottom scrollbar/border
  s32 end_y=h-wm_member_of.window_border_width;
  if (scrollbar_h.client_total_length_pixels){
    // have scrollbar
    s32 scrollbar_height=wm_member_of.bitmaps[wm_member_of.bitmap_scrollbar_horizontal_background]->h;
    end_y-=scrollbar_height;
  }
  return end_y;
}


void rtt_window::new_item(int index,string &text){
  items.push_back(combo_box_item(index,text));
  //  int scrollabe_height=(int)items.size()*COMBO_BOX_ITEM_SPACING+wm_member_of.window_title_bar_height+wm_member_of.window_border_width;
  int scrollabe_height=(int)items.size()*COMBO_BOX_ITEM_SPACING;
  add_scrollbars(0,scrollabe_height);
//  make_bitmap();
//  draw_furniture();
}


void rtt_window::add_combo_items(vector<string> &items){
  for(u32 i=0;i<items.size();i++){
    new_item(i,items[i]);
  }
}


void rtt_window::add_scrollbars(int _client_scrollable_width,int _client_scrollable_height){
  scrollbar_h.client_total_length_pixels=_client_scrollable_width;
  scrollbar_v.client_total_length_pixels=_client_scrollable_height;
}


void rtt_window::add_multiple_select_box(s32 x,s32 y,s32 w, s32 h, vector<string> &entries){
  //window_manager &_wm_member_of,int _id,const char*_title,int _x,int _y,int _w,int _h,COLORREF _window_background_col,win_cb _init_fn,win_cb _paint_fn,int window_type,int _client_scrollable_width,int _client_scrollable_height);
  char rtt[256];
  s32 n=multiple_select_boxes.size();
  sprintf(rtt,"multiple_select_box_%d",n);
  rtt_window *win=new rtt_window(wm_member_of,
                                 n, rtt, x, y, w, h, black_c, 0, 0,
                                 WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST,
                                 w,
                                 n*COMBO_BOX_ITEM_SPACING);
  for(u32 i=0;i<entries.size();i++){
    win->new_item(i,entries[i]);
  }
  win->paint();
  draw_multiple_select_box(*win, true);
  multiple_select_boxes.push_back(unique_ptr<rtt_window>(win));
}


void rtt_window::add_text_edit_box(string current_text,
                                   string name, 
                                   s32 x,s32 y,s32 w,s32 h,
                                   vector<string> entries_to_select,
                                   s32 current_selected_index,
                                   bool render_with_window_scroll_offset){
  text_edit_boxes.push_back(unique_ptr<rtt_text_edit_box>(new rtt_text_edit_box(*this, current_text, name,  x, y, w, h, entries_to_select, current_selected_index, render_with_window_scroll_offset)));
}


void rtt_window::add_button(string current_text,
                            string name,
                            std::function<void(RTTXCB&)> &&click_func,
                            s32 x,s32 y,s32 w,s32 h,
                            bool render_with_window_scroll_offset){
  buttons.push_back(unique_ptr<rtt_button>(new rtt_button(*this, current_text, name, std::move(click_func), x, y, w, h, render_with_window_scroll_offset)));
}


rtt_pane& rtt_window::add_pane(string name, 
                          paintfn_type &&_paint_fn,
                          clickfn_type &&_click_fn,
                          mouse_move_fn_type &&_mouse_move_fn,
                          s32 x,s32 y,s32 w,s32 h,
                          bool render_with_window_scroll_offset){
  panes.push_back(unique_ptr<rtt_pane>(new rtt_pane(*this, name, std::move(_paint_fn), std::move(_click_fn), std::move(_mouse_move_fn), x, y, w, h, render_with_window_scroll_offset)));
  last_pane_index=panes.size()-1;
  return *(panes.back());
}


// get dimensions from bitmap width
u32 rtt_window::add_knob2(string name, 
                               s32 x,
                               s32 y,
                               s32 _start_frame,
                               float _degrees_start,
                               float _degrees_end,
                               string _group,
                               string _desc,
                               bool _dont_reload_me){
  unique_ptr<RTTXCB> knob_bitmap(new RTTXCB(wm_member_of.device_context,name.data()));
  panes.push_back(unique_ptr<rtt_pane>(new rtt_pane(*this, name+"::pane", NULL, NULL, NULL, x, y, knob_bitmap->w, knob_bitmap->w, true)));
  panes.back()->add_dial(knob_bitmap, 0, 0, _start_frame, _degrees_start, _degrees_end, _group, _desc, _dont_reload_me);
  return panes.size()-1;
}


// get dimensions from bitmap width
u32 rtt_window::add_switch2(string name, 
                                  s32 x,
                                  s32 y,
                                  string _group,
                                  string _desc,
                                  bool is_momentary,
                                  bool _is_flashing,
                                  bool _dont_reload_me){
  unique_ptr<RTTXCB> knob_bitmap(new RTTXCB(wm_member_of.device_context,name.data()));
  panes.push_back(unique_ptr<rtt_pane>(new rtt_pane(*this, name+"::pane", NULL, NULL, NULL, x, y, knob_bitmap->w, knob_bitmap->h/SWITCH_KNOB_BITMAP_NUM_FRAMES, true)));
  panes.back()->add_switch(knob_bitmap, 0, 0, _group, _desc, is_momentary, _is_flashing, _dont_reload_me);
  return panes.size()-1;
}


void rtt_window::get_bitmaps(RTTXCB*&tb,RTTXCB*&bb,RTTXCB*&wlb,RTTXCB*&wrb,RTTXCB*&wb){
  int wbw=window_manager::window_border_width;
  //  u32 new_width=w-2*wbw;
  u32 new_width=w;
  if (is_selected){
    if (title_bar_selected==0){
      // make a local copy of the global window manager selected title bar
      title_bar_selected=new RTTXCB(*wm.bitmaps[window_manager::bitmap_window_title_bar_selected]);
    }
    bb=wm.bitmaps[window_manager::bitmap_window_bottom_border_selected];
    wlb=wm.bitmaps[window_manager::bitmap_window_left_border_selected];
    wrb=wm.bitmaps[window_manager::bitmap_window_right_border_selected];
    wb=wm.bitmaps[window_manager::bitmap_window_buttons_selected];
    if (last_title_bar_width_selected!=new_width){
      last_title_bar_width_selected=new_width;
      title_bar_selected->resample(new_width,title_bar_selected->h,0,0,RESIZE_FROM_ORIG_BMP,RESIZE_FROM_ORIG_BMP);
    }
    tb=title_bar_selected;
  }else{
    if (title_bar_deselected==0){
      // make a local copy of the global window manager deselected title bar
      title_bar_deselected=new RTTXCB(*wm.bitmaps[window_manager::bitmap_window_title_bar_deselected]);
    }
    bb=wm.bitmaps[window_manager::bitmap_window_bottom_border_deselected];
    wlb=wm.bitmaps[window_manager::bitmap_window_left_border_deselected];
    wrb=wm.bitmaps[window_manager::bitmap_window_right_border_deselected];
    wb=wm.bitmaps[window_manager::bitmap_window_buttons_deselected];
    if (last_title_bar_width_deselected!=new_width){
      last_title_bar_width_deselected=new_width;
      title_bar_deselected->resample(new_width,title_bar_deselected->h,0,0,RESIZE_FROM_ORIG_BMP,RESIZE_FROM_ORIG_BMP);
    }
    tb=title_bar_deselected;
  }
}


bool rtt_window::check_inside_horizontal_scroll_bar(pt &p){
  if (scrollbar_h.client_total_length_pixels){
    int xo=p.x-x;
    int yo=p.y-y+h;
    if (xo>=scrollbar_h.grab_bar_xstart && xo<scrollbar_h.grab_bar_xend){
      if (yo>=scrollbar_h.grab_bar_ystart && yo<scrollbar_h.grab_bar_yend){
        return true;
      }
    }
  }
  return false;
}


//void rtt_window::paint_controls(){
//  // paint controls
//  for(u32 i=0;i<panes.size();i++){
//    rtt_pane &p=*panes[i];
//    p.paint();
//    draw_pane(p,false);
//  }
//}



void rtt_window::draw_controls(){
  draw_text_edit_boxes();
  draw_multiple_select_boxes();
  draw_buttons();
  draw_panes();
}


void rtt_window::draw_multiple_select_boxes(){
  // draw controls like text edit boxes etc, so that they are rendered with the correct scroll offset if specified
  u32 n=multiple_select_boxes.size();
  for(u32 i=0;i<n;i++){
    rtt_window &w_msb=*multiple_select_boxes[i];
    //    w_msb.paint();//  RTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT TODODODODODOODODODO remove paint fn  here
    draw_multiple_select_box(w_msb,false);
  }
}
void rtt_window::draw_multiple_select_box(rtt_window &w,bool update_window_rect_on_screen_too){
  bool render_with_window_scroll_offset=true;
  draw_rttxcb_clipped_to_workarea(w.x, w.y, w.w, w.h, w, update_window_rect_on_screen_too, render_with_window_scroll_offset);
}


void rtt_window::draw_text_edit_boxes(){
  // draw controls like text edit boxes etc, so that they are rendered with the correct scroll offset if specified
  for(u32 i=0;i<text_edit_boxes.size();i++){
    rtt_text_edit_box &t=*text_edit_boxes[i];
    //    t.paint();
    draw_text_edit_box(t,false);
  }
}
void rtt_window::draw_text_edit_box(rtt_text_edit_box &t,bool update_window_rect_on_screen_too){
  draw_rttxcb_clipped_to_workarea(t.x, t.y, t.w, t.h, t, update_window_rect_on_screen_too, t.render_with_window_scroll_offset);
}


void rtt_window::draw_buttons(){
  // draw controls like text edit boxes etc, so that they are rendered with the correct scroll offset if specified
  for(u32 i=0;i<buttons.size();i++){
    rtt_button &b=*buttons[i];
    //    b.paint();
    draw_button(b,false);
  }
}
void rtt_window::draw_button(rtt_button &b,bool update_window_rect_on_screen_too){
  draw_rttxcb_clipped_to_workarea(b.x, b.y, b.w, b.h, b, update_window_rect_on_screen_too, b.render_with_window_scroll_offset);
}


void rtt_window::draw_panes(){
  // draw controls like text edit boxes etc, so that they are rendered with the correct scroll offset if specified
  for(u32 i=0;i<panes.size();i++){
    rtt_pane &p=*panes[i];
    //    p.paint();
    draw_pane(p,false);
  }
}
void rtt_window::draw_pane(rtt_pane &p,bool update_window_rect_on_screen_too){
  draw_rttxcb_clipped_to_workarea(p.x, p.y, p.w, p.h, p, update_window_rect_on_screen_too, p.render_with_window_scroll_offset);
}
  
 
void rtt_window::draw_rttxcb_clipped_to_workarea(s32 rect_x,s32 rect_y,s32 rect_w,s32 rect_h,RTTXCB &rttxcb,bool update_window_rect_on_screen_too,bool render_with_window_scroll_offset){
  s32 x_teb=rect_x+wm_member_of.window_border_width;
  s32 title_bar_h=wm_member_of.window_title_bar_height;
  s32 y_teb=rect_y+title_bar_h;
  s32 w_teb=rect_w;
  s32 h_teb=rect_h;
  s32 x_source_offset=0;
  s32 y_source_offset=0;
  if (render_with_window_scroll_offset){
    x_teb-=horizontal_display_offset;
    y_teb-=vertical_display_offset;
  }
  if (x_teb<wm_member_of.window_border_width){
    // clip left
    s32 overlap=wm_member_of.window_border_width-x_teb;
    w_teb-=overlap;
    x_source_offset+=overlap;
    x_teb=wm_member_of.window_border_width;
    if (w_teb<0){
      return;
    }
  }
  if (y_teb<title_bar_h){
    //clip top
    s32 overlap=title_bar_h-y_teb;
    h_teb-=overlap;
    y_teb=title_bar_h;
    y_source_offset+=overlap;
    if (h_teb<0){
      return;
    }
  }
  s32 x_teb_end=x_teb+w_teb;
  s32 vwex=visible_workarea_end_x();
  // clip right
  if (x_teb_end>vwex){
    w_teb-=x_teb_end-vwex;
    if (w_teb<=0){
      return;
    }
  }
  s32 y_teb_end=y_teb+h_teb;
  s32 vwey=visible_workarea_end_y();
  if (y_teb_end>vwey){
    //clip bottom
    h_teb-=y_teb_end-vwey;
    if (h_teb<=0){
      return;
    }
  }
  blit_bitmap_to_self(rttxcb, x_teb, y_teb, w_teb, h_teb, x_source_offset,y_source_offset,false);
  if (update_window_rect_on_screen_too){
    s32 screen_x=x+x_teb;
    s32 screen_y=y-y_teb-h_teb;
    //    RTTXCB_debug_mode=true;
    render_exposed_screen_rectangle(screen_x,screen_y,w_teb,h_teb);
    //    RTTXCB_debug_mode=false;
  }
}



void rtt_window::draw_furniture(){
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
      draw_client_window_furniture();
      break;
    case WINDOW_DISPLAY_TYPE_PANE_WINDOW:
      draw_pane_window_furniture();
      break;
    case WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST: 
    case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST: 
      draw_text_selection_dropdown_list_window_furniture();
      break;
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      draw_combo_box_window_furniture();
      break;
  }
}


void rtt_window::draw_title_bar(){
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
      if (draw_client_title_bar()){
        wm_member_of.wm_redraw_exposed_screen_rectangle(*this);// todo redraw only titlebar region
      }
      break;
    case WINDOW_DISPLAY_TYPE_PANE_WINDOW:
      draw_pane_title_bar();
      break;
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      if (draw_combo_box_title_bar()){
        wm_member_of.wm_redraw_exposed_screen_rectangle(*this);// todo redraw only titlebar region
      }
      break;
  }
}

#define TITLE_BAR_TEXT_START_X_OFFSET 20
#define TITLE_BAR_TEXT_START_Y_OFFSET 5

void rtt_window::draw_client_window_furniture(){
  RTTXCB *tb,*bb,*wlb,*wrb,*wb;
  get_bitmaps(tb, bb, wlb, wrb, wb);
  int wbw=window_manager::window_border_width;
  blit_bitmap_to_self(tb ,wbw, h  , w-2*wbw, tb->h ,0,0);// title_bar
  blit_bitmap_to_self(bb ,wbw, wbw, w-2*wbw, wbw   ,0,0);// bottom border

  COLORREF col=grey80_cr;
  if (is_selected){
    col=white_cr;
  }
  tahoma8_bold.p(*this,TITLE_BAR_TEXT_START_X_OFFSET,w,h-TITLE_BAR_TEXT_START_Y_OFFSET,col,title.data());
  blit_bitmap_to_self(wlb ,0     ,h   ,wbw ,h   ,0 ,0         );// left border
  blit_bitmap_to_self(wlb ,0     ,wbw ,wbw ,wbw ,0 ,wlb->h-wbw);// left bottom corner
  blit_bitmap_to_self(wrb ,w-wbw ,h   ,wbw ,h   ,0 ,0         );// right border
  blit_bitmap_to_self(wrb ,w-wbw ,wbw ,wbw ,wbw ,0 ,wrb->h-wbw);// right bottom corner
  blit_bitmap_to_self(wb  ,w-wb->w-WINDOW_BUTTON_X_OFFSET,h-WINDOW_BUTTON_Y_OFFSET,wb->w,wb->h,0,0); // buttons

  window_display_width_pixels=w-wbw*2;
  int sbx=wbw;
  int sby=wbw;
  int sbw=w-wbw*2;
  if (scrollbar_v.client_total_length_pixels){
    window_display_width_pixels-=DEFAULT_SCROLL_BAR_WIDTH;
    sbw-=DEFAULT_SCROLL_BAR_WIDTH;
  }
  scrollbar_h.draw(*this,sbx,sby,sbw,window_display_width_pixels,horizontal_display_offset,SB_DRAW_HORIZONTAL);

  window_display_height_pixels=h-wbw-tb->h;
  sbx=w-wbw-DEFAULT_SCROLL_BAR_WIDTH;
  sby=wbw+DEFAULT_SCROLL_BAR_WIDTH;
  int sbh=h-wbw-tb->h;
  if (scrollbar_h.client_total_length_pixels){
    window_display_height_pixels-=DEFAULT_SCROLL_BAR_WIDTH;
    sbh-=DEFAULT_SCROLL_BAR_WIDTH;
  }
  scrollbar_v.draw(*this,sbx,sby,sbh,window_display_height_pixels,vertical_display_offset,SB_DRAW_VERTICAL);
}


void rtt_window::draw_pane_window_furniture(){
  // draw pane border as normal window border but rendered half width
  RTTXCB *tb,*bb,*wlb,*wrb,*wb;
  get_bitmaps(tb, bb, wlb, wrb, wb);
  int wbw=window_manager::window_border_width>>1;
  blit_bitmap_to_self(wlb ,0     ,h   ,wbw     ,h     ,0   ,0          ); // left border
  blit_bitmap_to_self(wlb ,0     ,wbw ,wbw     ,wbw   ,0   ,wlb->h-wbw ); // left bottom corner
  blit_bitmap_to_self(wrb ,w-wbw ,h   ,wbw     ,h     ,wbw ,0          ); // right border
  blit_bitmap_to_self(wrb ,w-wbw ,wbw ,wbw     ,wbw   ,wbw ,wrb->h-wbw ); // right bottom corner
  blit_bitmap_to_self(tb  ,wbw   ,h   ,w-2*wbw ,tb->h ,0   ,0          ); // title bar
  blit_bitmap_to_self(bb  ,wbw   ,wbw ,w-2*wbw ,wbw   ,0   ,wbw        ); // bottom border
}


void rtt_window::draw_text_selection_dropdown_list_window_furniture(){
  // draw pane border as normal window border but rendered half width
  RTTXCB *tb,*bb,*wlb,*wrb,*wb;
  get_bitmaps(tb, bb, wlb, wrb, wb);
  int wbw=window_manager::window_border_width>>1;
  blit_bitmap_to_self(wlb ,0     ,h   ,wbw     ,h     ,0   ,0          ); // left border
  blit_bitmap_to_self(wlb ,0     ,wbw ,wbw     ,wbw   ,0   ,wlb->h-wbw ); // left bottom corner
  blit_bitmap_to_self(wrb ,w-wbw ,h   ,wbw     ,h     ,wbw ,0          ); // right border
  blit_bitmap_to_self(wrb ,w-wbw ,wbw ,wbw     ,wbw   ,wbw ,wrb->h-wbw ); // right bottom corner
  //  blit_bitmap_to_self(tb  ,wbw   ,h   ,w-2*wbw ,tb->h ,0   ,0          ); // title bar
  blit_bitmap_to_self(tb  ,wbw   ,h   ,w-2*wbw ,wbw   ,0   ,0          ); // title bar
  blit_bitmap_to_self(bb  ,wbw   ,wbw ,w-2*wbw ,wbw   ,0   ,wbw        ); // bottom border
  {
    int wbw=wm_member_of.window_border_width;
    int wbw2=wbw>>1;
    window_display_height_pixels=h-wbw2;
    int sbx=w-wbw2-DEFAULT_SCROLL_BAR_WIDTH;
    int sby=wbw2;
    int sbw=h-wbw;
    //    int sbl=window_display_height_pixels-wm_member_of.window_title_bar_height;
    int scroll_bar_rendered_height=window_display_height_pixels-wbw2;
    int visible_window_workarea_height=h-wbw;
    scrollbar_v.draw(*this,sbx,sby,scroll_bar_rendered_height,visible_window_workarea_height,vertical_display_offset,SB_DRAW_VERTICAL);
  }
}


void rtt_window::draw_combo_box_window_furniture(){
  draw_pane_window_furniture();
  tahoma8_bold.p(*this,TITLE_BAR_TEXT_START_X_OFFSET,w,h-TITLE_BAR_TEXT_START_Y_OFFSET,white_cr,title.data());
  int wbw=wm_member_of.window_border_width;
  int wbw2=wbw>>1;
  window_display_height_pixels=h-wbw2; 
  int sbx=w-wbw2-DEFAULT_SCROLL_BAR_WIDTH;
  int sby=wbw2;
  int sbw=h-wbw;
  int scroll_bar_rendered_height=window_display_height_pixels-wm_member_of.window_title_bar_height;
  int visible_window_workarea_height=h-wbw-wm_member_of.window_title_bar_height;
  scrollbar_v.draw(*this,sbx,sby,scroll_bar_rendered_height,visible_window_workarea_height,vertical_display_offset,SB_DRAW_VERTICAL);
}


bool rtt_window::draw_client_title_bar(){
  bool has_focus_changed=false;
  RTTXCB *tb,*bb,*wlb,*wrb,*wb;
  get_bitmaps(tb, bb, wlb, wrb, wb);
  int wbw=window_manager::window_border_width;
  blit_bitmap_to_self(tb ,wbw, h  , w-2*wbw, tb->h ,0,0);// title_bar
  //  blit_bitmap_to_self(bb ,wbw, wbw, w-2*wbw, wbw   ,0,0);// bottom border

  COLORREF col=grey80_cr;
  if (is_selected){
    col=white_cr;
  }
  tahoma8_bold.p(*this,TITLE_BAR_TEXT_START_X_OFFSET,w,h-TITLE_BAR_TEXT_START_Y_OFFSET,col,title.data());
  blit_bitmap_to_self(wlb ,0     ,h   ,wbw ,h   ,0 ,0         );// left border
  blit_bitmap_to_self(wlb ,0     ,wbw ,wbw ,wbw ,0 ,wlb->h-wbw);// left bottom corner
  blit_bitmap_to_self(wrb ,w-wbw ,h   ,wbw ,h   ,0 ,0         );// right border
  blit_bitmap_to_self(wrb ,w-wbw ,wbw ,wbw ,wbw ,0 ,wrb->h-wbw);// right bottom corner
  blit_bitmap_to_self(wb  ,w-wb->w-WINDOW_BUTTON_X_OFFSET,h-WINDOW_BUTTON_Y_OFFSET,wb->w,wb->h,0,0); // buttons

  if (last_is_selected!=is_selected){
    has_focus_changed=true;
  }
  last_is_selected=is_selected;
  return has_focus_changed;
}


void rtt_window::draw_pane_title_bar(){
  // draw pane border as normal window border but rendered half width
  RTTXCB *tb,*bb,*wlb,*wrb,*wb;
  get_bitmaps(tb, bb, wlb, wrb, wb);
  int wbw=window_manager::window_border_width>>1;
  blit_bitmap_to_self(wlb ,0     ,h   ,wbw     ,h     ,0   ,0          ); // left border
  blit_bitmap_to_self(wlb ,0     ,wbw ,wbw     ,wbw   ,0   ,wlb->h-wbw ); // left bottom corner
  blit_bitmap_to_self(wrb ,w-wbw ,h   ,wbw     ,h     ,wbw ,0          ); // right border
  blit_bitmap_to_self(wrb ,w-wbw ,wbw ,wbw     ,wbw   ,wbw ,wrb->h-wbw ); // right bottom corner
  blit_bitmap_to_self(tb  ,wbw   ,h   ,w-2*wbw ,tb->h ,0   ,0          ); // title bar
  //  blit_bitmap_to_self(bb  ,wbw   ,wbw ,w-2*wbw ,wbw   ,0   ,wbw        ); // bottom border
}


bool rtt_window::draw_combo_box_title_bar(){
  bool has_focus_changed=false;
  draw_pane_window_furniture();
  tahoma8_bold.p(*this,TITLE_BAR_TEXT_START_X_OFFSET,w,h-TITLE_BAR_TEXT_START_Y_OFFSET,white_cr,title.data());

  if (last_is_selected!=is_selected){
    has_focus_changed=true;
  }
  last_is_selected=is_selected;
  return has_focus_changed;
}


void rtt_window::blit_bitmap_to_workarea_only_no_furniture_offset(RTTXCB *rttgdi_source){
  //  int screen_x=wm_member_of.window_border_width;
  //  int screen_y=h-wm_member_of.bitmaps[wm_member_of.bitmap_window_title_bar_selected]->h;
  //  int screen_w=w-2*wm_member_of.window_border_width;
  //  int screen_h=h-wm_member_of.bitmaps[wm_member_of.bitmap_window_title_bar_selected]->h-wm_member_of.window_border_width;
  //  int copy_from_rectangle_x=0;
  //  int copy_from_rectangle_y=0;
  //  if (scrollbar_h.client_total_length_pixels){
  //    screen_h-=DEFAULT_SCROLL_BAR_WIDTH;
  //  }
  //  if (scrollbar_v.client_total_length_pixels){
  //    screen_w-=DEFAULT_SCROLL_BAR_WIDTH;
  //  }
  blit_bitmap_to_self(rttgdi_source,0,h,w,h,horizontal_display_offset,0+vertical_display_offset);
}


void rtt_window::plot_vertically_stretched_button_style_bitmap_with_end_caps(RTTXCB *b_middle,RTTXCB *b_caps,int x,int y,int h,int border_h){
  blit_bitmap_to_self(b_caps   ,x ,y+border_h   ,b_caps->w   ,border_h     ,0 ,b_caps->h-border_h);// bottom border
  blit_bitmap_to_self(b_caps   ,x ,y+h          ,b_caps->w   ,border_h     ,0 ,0                 );// top border
  blit_bitmap_to_self(b_middle ,x ,y+h-border_h ,b_middle->w ,h-2*border_h ,0 ,0                 );// top border
}


void rtt_window::plot_horizontally_stretched_button_style_bitmap_with_end_caps(RTTXCB *b_middle,RTTXCB *b_caps,int x,int y,int w,int border_w){
  blit_bitmap_to_self(b_caps,   x+w-border_w, y, border_w    , b_caps->h  , 2*border_w, 0);// left border
  blit_bitmap_to_self(b_caps,   x,            y, border_w    , b_caps->h  , 0,          0);// left border
  blit_bitmap_to_self(b_middle, x+border_w,   y, w-2*border_w, b_middle->h, 0,          0);// middle
}


void rtt_window::plot_raised_taskbar_item(int x,int y,int w){
  // assume bitmap is 2px border, so stretch to fit
  RTTXCB*b=wm.bitmaps[window_manager::bitmap_task_bar_item];
  blit_bitmap_to_self(b ,x                            ,y ,RAISED_ITEM_BORDER_WIDTH        ,b->h ,0                          ,0);// left border
  blit_bitmap_to_self(b ,x+RAISED_ITEM_BORDER_WIDTH   ,y ,b->w-2*RAISED_ITEM_BORDER_WIDTH ,b->h ,RAISED_ITEM_BORDER_WIDTH   ,0);// title bar stretched
  blit_bitmap_to_self(b ,x+w-RAISED_ITEM_BORDER_WIDTH ,y ,RAISED_ITEM_BORDER_WIDTH        ,b->h ,2*RAISED_ITEM_BORDER_WIDTH ,0);// left border
}


void rtt_window::handle_window_animated_elements(s32 action){
  switch(action){
    case WINDOW_ANIMATION_ACTION_TOGGLE_CARET_1:
      for(u32 i=0;i<text_edit_boxes.size();i++){
        rtt_text_edit_box &t=*text_edit_boxes[i];
        switch(t.caret_style){
          case CARET_STYLE_SOLID_1:
          case CARET_STYLE_SOLID_2:
            t.caret_style=CARET_STYLE_SOLID_1;
            t.paint();
            draw_text_edit_box(t,true);
            break;
        }
      }
      break;
    case WINDOW_ANIMATION_ACTION_TOGGLE_CARET_2:
      for(u32 i=0;i<text_edit_boxes.size();i++){
        rtt_text_edit_box &t=*text_edit_boxes[i];
        switch(t.caret_style){
          case CARET_STYLE_SOLID_1:
          case CARET_STYLE_SOLID_2:
            t.caret_style=CARET_STYLE_SOLID_2;
            t.paint();
            draw_text_edit_box(t,true);
            break;
        }
      }
      break;
  }
}


void rtt_window::key(u32 key_action,u32 OS_keycode,bool is_ascii,u8 ASCII){
  if (currently_selected_text_edit_box!=-1){
    rtt_text_edit_box &t=*text_edit_boxes[currently_selected_text_edit_box];
    u32 action=t.key(key_action,OS_keycode,is_ascii,ASCII);
    switch(action){
      case RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT:
        t.paint();
        draw_text_edit_box(t,true);
        break;
      case RTT_TEXT_EDIT_BOX_RETURN_ACTION_FINISHED:
        currently_selected_text_edit_box=-1;
        t.caret_style=CARET_STYLE_DESELECTED;
        t.paint();
        draw_text_edit_box(t,true);
        break;
      case RTT_TEXT_EDIT_BOX_RETURN_ACTION_NEXT_EDITBOX:{
        currently_selected_text_edit_box++;
        if (currently_selected_text_edit_box>=text_edit_boxes.size()){
          currently_selected_text_edit_box=0;
        }
        t.caret_style=CARET_STYLE_DESELECTED;
        t.paint();
        draw_text_edit_box(t,true);
        rtt_text_edit_box &tn=*text_edit_boxes[currently_selected_text_edit_box];
        tn.editing_state=RTT_EDIT_BOX_EDITING_STATE_EDITING;
        tn.caret_style=CARET_STYLE_SOLID_1;
        tn.paint();
        draw_text_edit_box(tn,true);
        flash_my_carrot=currently_selected_text_edit_box;
        break;
      }
      case RTT_TEXT_EDIT_BOX_RETURN_ACTION_PREVIOUS_EDITBOX:{
        currently_selected_text_edit_box--;
        if (currently_selected_text_edit_box<0){
          if (text_edit_boxes.size()>0){
            currently_selected_text_edit_box=text_edit_boxes.size()-1;
          }else{
            currently_selected_text_edit_box=0;
          }
        }
        t.caret_style=CARET_STYLE_DESELECTED;
        t.paint();
        draw_text_edit_box(t,true);
        rtt_text_edit_box &tn=*text_edit_boxes[currently_selected_text_edit_box];
        tn.editing_state=RTT_EDIT_BOX_EDITING_STATE_EDITING;
        tn.caret_style=CARET_STYLE_SOLID_1;
        tn.paint();
        draw_text_edit_box(tn,true);
        flash_my_carrot=currently_selected_text_edit_box;
        break;
      }
    }
  }
}


void rtt_window::mouse_move(s32 x_mouse,s32 y_mouse){
  // check multiple selection boxes for mouse movement
  if (!is_selected){
    return;
  }
  u32 n=multiple_select_boxes.size();
  for(u32 i=0;i<n;i++){
    rtt_window &w_msb=*multiple_select_boxes[i];
    transform_multiple_select_window_coordiantes(w_msb, TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_NATIVE_WINDOW_TO_WINDOW_MANAGER_COORDINATES);
    int dy=y_mouse-last_mouse_pos.y;
    transform_multiple_select_window_coordiantes(w_msb, TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_WINDOW_MANAGER_TO_NATIVE_WINDOW_COORDINATES);
    //    printf("shifted coords of multiple select box to %d %d\n",w_msb.x,w_msb.y);
    if (w_msb.grabbed_v_scrollbar){
      if (dy!=0){
        w_msb.vertical_display_offset=w_msb.scrollbar_v.update_scroll_pos(-dy,w_msb.window_display_height_pixels);
        if (w_msb.vertical_display_offset<0){
          w_msb.vertical_display_offset=0;
        }
        w_msb.paint();
        draw_multiple_select_box(w_msb,true);
        if (SEE_USER_IO_EVENTS){
          printf("scroll scroll %d %d\n",last_mouse_pos.y,y_mouse);
        }
      }
    }
  }
  // pass mouse move on to panes
  s32 x_workarea=x_mouse-x-wm_member_of.window_border_width+horizontal_display_offset;
  s32 y_workarea=y-y_mouse-wm_member_of.window_title_bar_height+vertical_display_offset;
  for(u32 i=0;i<panes.size();i++){
    rtt_pane &p=*panes[i];
    if (p.mouse_move(x_workarea, y_workarea)){
      p.paint();
      draw_pane(p,true);
    }
  }
}


void rtt_window::mouse_click_in_window_area(s32 butt, s32 x_click,s32 y_click){
  last_mouse_pos=pt(x_click,y_click);  // main window now has y increasing up the screen
  if (!is_selected){
    return;
  }
  if (mouse_click_test_edit_boxes(butt,x_click,y_click)){
    // was handled by an edit box
    return;
  }
  if (mouse_click_multiple_select_boxes(butt,x_click,y_click)){
    // was handled by an edit box
    return;
  }
  if (mouse_click_buttons(butt,x_click,y_click)){
    // was handled by a button
    return;
  }
  if (mouse_click_panes(butt,x_click,y_click)){
    // was handled by a pane
    return;
  }
}


void rtt_window::transform_multiple_select_window_coordiantes(rtt_window &w_msb,s32 transform_type){
  // transform multiple select box coordinates so that WM relative fns like check_combo_mouse_posn can be used
  bool title_bar_present=false;
  switch(window_type){
    case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
    case WINDOW_DISPLAY_TYPE_COMBO_BOX:
      title_bar_present=true;
      break;
    case WINDOW_DISPLAY_TYPE_PANE_WINDOW:
    case WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST:
    case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST:
      title_bar_present=false;
      break;
  }
  int wbw=wm_member_of.window_border_width;
  int wbw2=wbw>>1;
  int wtbw;
  if (title_bar_present){
    wtbw=wm_member_of.window_title_bar_height;
  }else{
    wtbw=wbw2;
  }
  s32 transform_x=x+wbw2-horizontal_display_offset;
  s32 transform_y=y-wtbw+vertical_display_offset;
  switch(transform_type){
    case TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_NATIVE_WINDOW_TO_WINDOW_MANAGER_COORDINATES:
      w_msb.x+=transform_x;
      w_msb.y=transform_y-w_msb.y;
      break;
    case TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_WINDOW_MANAGER_TO_NATIVE_WINDOW_COORDINATES:
      w_msb.x-=transform_x;
      w_msb.y=transform_y-w_msb.y;
      break;
  }
}


bool rtt_window::mouse_click_multiple_select_boxes(s32 butt, s32 x_click,s32 y_click){
  // test edit boxes for caret positioning or drop menu creation
  //  s32 x_workarea=x_click-x-wm_member_of.window_border_width+horizontal_display_offset;
  //  s32 y_workarea=y-y_click-wm_member_of.window_title_bar_height+vertical_display_offset;
  switch(butt){
    case MOUSE_LEFT_DOWN:
    case MOUSE_RIGHT_DOWN:
      break;
    case MOUSE_LEFT_UP:
    case MOUSE_RIGHT_UP:
    case MOUSE_MIDDLE_UP:{
      u32 n=multiple_select_boxes.size();
      for(u32 i=0;i<n;i++){
        rtt_window &w_msb=*multiple_select_boxes[i];
        w_msb.grabbed_h_scrollbar=false;
        w_msb.grabbed_v_scrollbar=false;
      }
    }
    case MOUSE_LEFT_DOUBLE_CLICK:
    case MOUSE_MIDDLE_DOWN:
      return false;
      break;
  }
  u32 n=multiple_select_boxes.size();
  for(u32 i=0;i<n;i++){
    rtt_window &w_msb=*multiple_select_boxes[i];
    transform_multiple_select_window_coordiantes(w_msb, TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_NATIVE_WINDOW_TO_WINDOW_MANAGER_COORDINATES);
    if (SEE_GRAPHICS_CREATION_DEBUG_LOGGING){
      printf("shifted coords of multiple select box to %d %d\n",w_msb.x,w_msb.y);
    }
    bool res=w_msb.check_combo_mouse_posn(x_click,y_click,butt);
    if (w_msb.scrollbar_v.inside(w_msb,last_mouse_pos)){
      if (SEE_GRAPHICS_CREATION_DEBUG_LOGGING){
        printf("grabbed multiple select box scrollbar\n");
      }
      w_msb.grabbed_v_scrollbar=true;
      w_msb.scrollbar_v.display_offset_start=w_msb.vertical_display_offset;
    }

    transform_multiple_select_window_coordiantes(w_msb, TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_WINDOW_MANAGER_TO_NATIVE_WINDOW_COORDINATES);
    if (res){
      w_msb.paint();
      draw_multiple_select_box(w_msb,true);
      return true;
    }
  }
  return false;
}


bool rtt_window::mouse_click_buttons(s32 butt, s32 x_click,s32 y_click){
  // test buttons for click
  s32 x_workarea=x_click-x-wm_member_of.window_border_width+horizontal_display_offset;
  s32 y_workarea=y-y_click-wm_member_of.window_title_bar_height+vertical_display_offset;
  for(u32 i=0;i<buttons.size();i++){
    rtt_button &b=*buttons[i];
    switch(b.mouse_click(butt, x_workarea, y_workarea)){
      case RTT_BUTTON_MOUSE_CLICKED_BUTTON:
      case RTT_BUTTON_MOUSE_UNCLICKED_BUTTON:
        b.paint();
        draw_button(b,true);
        return true;
    }
  }
  return false;
}


bool rtt_window::mouse_click_panes(s32 butt, s32 x_click,s32 y_click){
  // test buttons for click
  s32 x_workarea=x_click-x-wm_member_of.window_border_width+horizontal_display_offset;
  s32 y_workarea=y-y_click-wm_member_of.window_title_bar_height+vertical_display_offset;
  for(u32 i=0;i<panes.size();i++){
    rtt_pane &p=*panes[i];
    //    //    p.paint();
    //    draw_pane(p,false);
    //    rtt_button &b=*buttons[i];
    switch(p.mouse_click(butt, x_workarea, y_workarea)){
      case RTT_BUTTON_MOUSE_CLICKED_BUTTON:
      case RTT_BUTTON_MOUSE_UNCLICKED_BUTTON:
        p.paint();
        draw_pane(p,true);
        return true;
    }
  }
  return false;
}


bool rtt_window::mouse_click_test_edit_boxes(s32 butt, s32 x_click,s32 y_click){
  // test edit boxes for caret positioning or drop menu creation
  s32 x_workarea=x_click-x-wm_member_of.window_border_width+horizontal_display_offset;
  s32 y_workarea=y-y_click-wm_member_of.window_title_bar_height+vertical_display_offset;
  //  printf("offsets hdo=%d x_wo=%d vdo=%d y_wo=%d \n",horizontal_display_offset,x_workarea,vertical_display_offset,y_workarea);
  s32 unset_carets_elsewhere=-1;
  bool was_clicked_in_an_edit_box=false;
  for(u32 i=0;i<text_edit_boxes.size();i++){
    rtt_text_edit_box &t=*text_edit_boxes[i];
    switch(t.mouse_click(butt, x_workarea, y_workarea)){
      case RTT_EDIT_BOX_MOUSE_CLICK_RETURN_NOTHING:
        break;
      case RTT_EDIT_BOX_MOUSE_CLICK_RETURN_CLICKED_BUTTON:
        was_clicked_in_an_edit_box=true;
        break;
      case RTT_EDIT_BOX_MOUSE_CLICK_RETURN_CLICKED_IN_TEXT_EDIT_BOX_TEXT_AREA:
        {
          rtt_text_edit_box &t=*text_edit_boxes[i];
          t.caret_style=CARET_STYLE_SOLID_1;
          t.paint();
          draw_text_edit_box(t,true);
          currently_selected_text_edit_box=i;
          unset_carets_elsewhere=i;
          flash_my_carrot=i;
          was_clicked_in_an_edit_box=true;
        }
        break;
    }
  }
  if (unset_carets_elsewhere!=-1){
    // unset carets in other edit boxes
    for(u32 i=0;i<text_edit_boxes.size();i++){
      if (i==unset_carets_elsewhere){
        continue;
      }
      rtt_text_edit_box &t=*text_edit_boxes[i];
      t.caret_style=CARET_STYLE_DESELECTED;
      t.paint();
      draw_text_edit_box(t,true);
    }
  }
  return was_clicked_in_an_edit_box;
}


static void scroll_bar(){};


int rtt_scrollbar::update_scroll_pos(int dx,int window_display_width_pixels){
  int client_width_left_to_scroll=client_total_length_pixels-window_display_width_pixels;
  return display_offset_start+(int)((float)dx/(float)grab_bar_pixels_left_to_scroll*(float)client_width_left_to_scroll+0.5f);
}


bool rtt_scrollbar::inside(rtt_window &w,pt &p){
  if (client_total_length_pixels){
    int xo=p.x-w.x;
    int yo=p.y-w.y+w.h;
    if (xo>=grab_bar_xstart && xo<grab_bar_xend){
      if (yo>=grab_bar_ystart && yo<grab_bar_yend){
        return true;
      }
    }
  }
  return false;
}


void rtt_scrollbar::draw(rtt_window &w,int x,int y,int scrollbar_plot_length,int client_length,int &client_offset,int draw_style){
  if (client_total_length_pixels){
    int scrollbar_grab_bar_plot_space=scrollbar_plot_length-DEFAULT_SCROLL_BAR_WIDTH*2;
    float visible_fraction=(float)client_length/(float)client_total_length_pixels;

    int grab_bar_plot_length=(int)(visible_fraction*(float)scrollbar_grab_bar_plot_space+0.5f);
    if (grab_bar_plot_length<MINIMUM_SCROLLBAR_SIZE){
      grab_bar_plot_length=MINIMUM_SCROLLBAR_SIZE;
    }
    if (grab_bar_plot_length>scrollbar_grab_bar_plot_space){
      grab_bar_plot_length=scrollbar_grab_bar_plot_space;
    }

    max_display_offset=client_total_length_pixels-client_length;
    if (client_offset>max_display_offset){
      client_offset=max_display_offset;
    }
    if (client_offset<0){
      client_offset=0;
    }

    int grab_bar_scroll_region_size=scrollbar_grab_bar_plot_space-grab_bar_plot_length;
    grab_bar_pixels_left_to_scroll=scrollbar_grab_bar_plot_space-grab_bar_plot_length;

    if (draw_style==SB_DRAW_HORIZONTAL){
      int grab_bar_x;
      if (max_display_offset){
        grab_bar_x=x+DEFAULT_SCROLL_BAR_WIDTH+(int)((float)grab_bar_scroll_region_size*client_offset/(float)max_display_offset+0.5f);
      }else{
        grab_bar_x=x+DEFAULT_SCROLL_BAR_WIDTH;
      }
      int yp=y+DEFAULT_SCROLL_BAR_WIDTH;
      grab_bar_xstart=grab_bar_x;
      grab_bar_xend=grab_bar_x+grab_bar_plot_length;
      grab_bar_ystart=yp-DEFAULT_SCROLL_BAR_WIDTH;
      grab_bar_yend=yp;

      RTTXCB *bl=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_left];
      RTTXCB *br=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_right];
      RTTXCB *bb=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_horizontal_bar];
      RTTXCB *bm=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_horizontal_bar_middle];
      RTTXCB *bk=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_horizontal_background];
      w.blit_bitmap_to_self(bl ,x                                                ,yp ,DEFAULT_SCROLL_BAR_WIDTH      ,DEFAULT_SCROLL_BAR_WIDTH   ,0 ,0 );// left scrollbar button
      w.blit_bitmap_to_self(br ,x+scrollbar_plot_length-DEFAULT_SCROLL_BAR_WIDTH ,yp ,DEFAULT_SCROLL_BAR_WIDTH      ,DEFAULT_SCROLL_BAR_WIDTH   ,0 ,0 );// right scrollbar button
      w.blit_bitmap_to_self(bk ,x+DEFAULT_SCROLL_BAR_WIDTH                       ,yp ,scrollbar_grab_bar_plot_space ,DEFAULT_SCROLL_BAR_WIDTH   ,0 ,0 );// scrollbar background
      
      w.plot_horizontally_stretched_button_style_bitmap_with_end_caps(bm, bb, grab_bar_x, yp, grab_bar_plot_length, 2);// scrollbar grab-bar
    }else{
      int grab_bar_y;
      if (max_display_offset){
        grab_bar_y=y+DEFAULT_SCROLL_BAR_WIDTH+(int)((float)grab_bar_scroll_region_size*(max_display_offset-client_offset)/(float)max_display_offset+0.5f);
      }else{
        grab_bar_y=y+DEFAULT_SCROLL_BAR_WIDTH;
      }
      int yp=y+DEFAULT_SCROLL_BAR_WIDTH;
      grab_bar_xstart=x;
      grab_bar_xend=x+DEFAULT_SCROLL_BAR_WIDTH;
      grab_bar_ystart=grab_bar_y;
      grab_bar_yend=grab_bar_y+grab_bar_plot_length;

      RTTXCB *bd=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_down];
      RTTXCB *bu=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_up];
      RTTXCB *bb=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_vertical_bar];
      RTTXCB *bm=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_vertical_bar_middle];
      RTTXCB *bk=w.wm_member_of.bitmaps[window_manager::bitmap_scrollbar_vertical_background];
      w.blit_bitmap_to_self(bd ,x ,yp                                                ,DEFAULT_SCROLL_BAR_WIDTH ,DEFAULT_SCROLL_BAR_WIDTH      ,0 ,0 );// left scrollbar button
      w.blit_bitmap_to_self(bu ,x ,y+scrollbar_plot_length                           ,DEFAULT_SCROLL_BAR_WIDTH ,DEFAULT_SCROLL_BAR_WIDTH      ,0 ,0 );// right scrollbar button
      w.blit_bitmap_to_self(bk ,x ,y+scrollbar_plot_length-DEFAULT_SCROLL_BAR_WIDTH  ,DEFAULT_SCROLL_BAR_WIDTH ,scrollbar_grab_bar_plot_space ,0 ,0 );// scrollbar background
      w.plot_vertically_stretched_button_style_bitmap_with_end_caps(bm,bb,x,grab_bar_y,grab_bar_plot_length,2);// scrollbar grab-bar
    }
  }
}

