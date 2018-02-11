// contains some OS specifics to interface application with whatever renderer backend is used
// handles multiple window interface functionality, so that application only has to care about individual window geometry

#include <stdarg.h>
#include "RTTGDIXCB.h"
#include "OS_interface.h"
#include "main.h"
#include "RTT_2.h"
#include "RTT_colours.h"
#include <cassert>
#include <memory>
#include <algorithm>
#include "fonts/font_tahoma_8pt.h"
#include "RTT_keysym_values_used.h"
#include <sys/time.h>

window_manager wm;

int window_manager::bitmap_window_bottom_border_deselected=-1;
int window_manager::bitmap_window_bottom_border_selected=-1;
int window_manager::bitmap_window_left_border_deselected=-1;
int window_manager::bitmap_window_left_border_selected=-1;
int window_manager::bitmap_window_right_border_deselected=-1;
int window_manager::bitmap_window_right_border_selected=-1;
int window_manager::bitmap_window_title_bar_deselected=-1;
int window_manager::bitmap_window_title_bar_selected=-1;
int window_manager::bitmap_window_buttons_deselected=-1;
int window_manager::bitmap_window_buttons_selected=-1;
int window_manager::bitmap_task_bar_item=-1;
int window_manager::bitmap_button_down_arrow_pressed=-1;
int window_manager::bitmap_button_down_arrow=-1;
int window_manager::bitmap_button_up_arrow=-1;
int window_manager::bitmap_button_up_arrow_pressed=-1;
int window_manager::bitmap_task_bar_item_minimized_indicator=-1;
int window_manager::bitmap_scrollbar_down=-1;
int window_manager::bitmap_scrollbar_horizontal_background=-1;
int window_manager::bitmap_scrollbar_horizontal_bar=-1;
int window_manager::bitmap_scrollbar_left=-1;
int window_manager::bitmap_scrollbar_right=-1;
int window_manager::bitmap_scrollbar_up=-1;
int window_manager::bitmap_scrollbar_vertical_background=-1;
int window_manager::bitmap_scrollbar_vertical_bar=-1;
int window_manager::bitmap_scrollbar_vertical_bar_middle=-1;
int window_manager::bitmap_scrollbar_horizontal_bar_middle=-1;
int window_manager::bitmap_text_box=-1;
int window_manager::bitmap_button_pressed=-1;
int window_manager::bitmap_button_normal=-1;

int window_manager::window_border_width;
int window_manager::window_title_bar_height;

#define TASK_BAR_WIDTH 200
#define TASK_BAR_OFFSET_FROM_TOP_OF_SCREEN_TO_START_OF_WINDOW_ITEMS 200

//RTTXCB *back_buffer;

u32 GetTickCount(){
  struct timeval tv;
  if(gettimeofday(&tv, NULL) != 0)
    return 0;
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


void window_manager::set_main_window_geometry(int x,int y,int w,int h,bool hide_borders){
  main_w=w;
  main_h=h;
  //  main_x=x;
  //  main_y=y;
  main_x=0;// on X/XCB mouse coords are relative to the window origin, not the screen coords
  main_y=0;
  //  back_buffer=new RTTXCB(device_context,main_w,main_h,grey15_cr,"back buffer");
  //  back_buffer->win_set_main_window_geometry(x,y,w,h,hide_borders);
  //  task_bar=unique_ptr<rtt_window>(&new_window(10000,0,-TASK_BAR_WIDTH,main_h,TASK_BAR_WIDTH,h,grey80_cr,0,task_bar_paint,WINDOW_DISPLAY_TYPE_PANE_WINDOW,0,0));
  new_window(10000,"task bar",-TASK_BAR_WIDTH,main_h,TASK_BAR_WIDTH,h,grey80_cr,0,task_bar_paint,WINDOW_DISPLAY_TYPE_PANE_WINDOW,0,0);
}

window_manager::~window_manager(){
  printf("destructing window_manager this=%16x\n",(uint64_t)this);
  u32 n=(int)window_stack.size();
  for(int i=1;i<n;i++){
    rtt_window &w=*window_stack[i];
    printf("stack %d=%16x - %16x RTTXCB=\n",i,(uint64_t)window_stack[i].get());
  }
}


window_manager::window_manager(){
  main_w=0;
  main_h=0;
  window_border_width=0;
  window_selected=0;
  mouse_left_button_is_down=false;
  mouse_right_button_is_down=false;
  window_index_move_window=-1;
  window_index_resize_window_right_border=-1;
  window_index_resize_window_left_border=-1;
  window_index_resize_window_bottom_border=-1;
  window_index_resize_window_top_border=-1;
  window_index_grabbed_vertical_scrollbar=-1;
  window_index_grabbed_horizontal_scrollbar=-1;
  window_manager::window_title_bar_height=0;
  key_shift_pressed=false;
  key_control_pressed=false;
  key_alt_pressed=false;
}


rtt_window& window_manager::new_window(int id,
                                       const char*title,
                                       int x,
                                       int y,
                                       int w,
                                       int h,
                                       COLORREF background_col,
                                       win_cb init_fn,
                                       win_cb paint_fn,
                                       int window_type,
                                       int _client_scrollable_width,
                                       int _client_scrollable_height){
  rtt_window *win=new rtt_window(*this,id,title,x,y,w,h,background_col,init_fn,paint_fn,window_type,_client_scrollable_width,_client_scrollable_height);
  window_stack.push_back(unique_ptr<rtt_window>(win));
  //  window_stack.push_back(win);
  return *win;
}


void window_manager::init(RTT_DC dc){
  device_context=dc; // only place where OS rendering context should be specified
  if (bitmap_window_title_bar_selected==-1){
    load_window_bitmaps();
    setup_key_translate();
  }
  RTT_init();
}

bool can_print_debug_spam=false;

void window_manager::update(){
  static u32 updates=0;
  //  printf("window_manager::update() %d\n",updates++);
  if (updates>10){
    can_print_debug_spam=true;
  }
  if (updates==20){
    break_now=true;
  }
  RTT_update();
  static bool toggle_cursors=true;
  toggle_cursors^=true;
  window_stack.back()->handle_window_animated_elements(toggle_cursors?WINDOW_ANIMATION_ACTION_TOGGLE_CARET_1:WINDOW_ANIMATION_ACTION_TOGGLE_CARET_2);
}


void window_manager::render_window(int i){
  rtt_window &w=*window_stack[i];
  w.paint();
  w.blit_entire_bitmap_to_screen(w.x,w.y,main_h);
}


void window_manager::re_blit_window_to_screen(int i){
  rtt_window &w=*window_stack[i];
  //  w->paint();
  w.blit_entire_bitmap_to_screen(w.x,w.y,main_h);
}


void window_manager::wm_redraw_exposed_screen_rectangle(rtt_window &w){
  // adjust y so rectangle is +y screen up
  wm_redraw_exposed_screen_rectangle(w.x ,w.y-w.h, w.w, w.h);
}


void window_manager::wm_redraw_exposed_screen_rectangle(s32 x ,s32 y, s32 w, s32 h){
  //  back_buffer->start_render_to_backbuffer();
  backdrop->render_exposed_screen_rectangle(x,y,w,h);
  for(int i=1;i<(int)window_stack.size();i++){
    rtt_window &win=*window_stack[i];
    //    w->paint();
    //    if (!win.is_minimized){
    string coco=win.title;
    win.render_exposed_screen_rectangle(x,y,w,h);
    //    }
    //    w->blit_entire_bitmap_to_screen(w->x,w->y,main_h);
  }
  window_stack[0]->render_exposed_screen_rectangle(x,y,w,h); // always render task_bar ontop
  //  render_window(0); // render task_bar ontop always
  //  back_buffer->swap_back_buffer_to_main();
}


void window_manager::update_all_windows_contents(){
  for(int i=1;i<(int)window_stack.size();i++){
    window_stack[i]->paint();
  }
  window_stack[0]->paint();
}


void window_manager::render_all_windows_to_the_screen(){
  for(int i=1;i<(int)window_stack.size();i++){
    rtt_window &w=*window_stack[i];
    w.blit_entire_bitmap_to_screen(w.x,w.y,main_h);
  }
  rtt_window &w=*window_stack[0];
  w.blit_entire_bitmap_to_screen(w.x,w.y,main_h);
}


//void window_manager::set_backdrop(RTTXCB *_backdrop){
//  backdrop=_backdrop;
//}


void window_manager::determine_region_to_redraw_after_window_motion(u32 moved_window_index,s32 old_x,s32 old_y,s32 old_w,s32 old_h){
  // produce non overlapping screen rectangles to refresh for minimum redraw when window geometry changes
  // dumbly redraw both rects
  rtt_window &w=*window_stack[moved_window_index];
  rect_clipper rc;
  rc.calculate_strips_for_a_window_moving_assuming_new_position_is_already_fully_drawn(old_x,old_y,old_w,old_h,w.x,w.y,w.w,w.h);
  //  rc.window_moved_test();

  s32 w1_xmin=w.x;
  s32 w1_xmax=w.x+w.w;
  s32 w1_ymin=w.y-w.h;
  s32 w1_ymax=w.y;
  
  s32 w0_xmin=old_x;
  s32 w0_xmax=old_x+old_w;
  s32 w0_ymin=old_y-old_h;
  s32 w0_ymax=old_y;
  
  s32 min_x=min(w1_xmin,w0_xmin);
  s32 max_x=max(w1_xmax,w0_xmax);
  s32 min_y=min(w1_ymin,w0_ymin);
  s32 max_y=max(w1_ymax,w0_ymax);
  assert(max_y-min_y>0);
  assert(max_x-min_x>0);
  re_blit_window_to_screen(moved_window_index);
  //  rc.draw_debug_rect_strips();
#if 0
  render_exposed_screen_rectangle(min_x, min_y, max_x-min_x, max_y-min_y);
#else
  rc.redraw_exposed_screen_rectangle_strips(*this);
#endif
}


void window_manager::fake_window_move(){
  window_index_move_window=1;
  rtt_window &w=*window_stack[window_index_move_window];
  s32 old_x=w.x;
  s32 old_y=w.y;
  s32 old_w=w.w;
  s32 old_h=w.h;
  w.x=old_x-200;
  w.y=old_y+0;
  w.remember_size();
  determine_region_to_redraw_after_window_motion(window_index_move_window, old_x, old_y, old_w, old_h);
}


void expand_rectangle(s32 &x_use,s32 &y_use,s32 &w_use,s32 &h_use,s32 x_expand_to,s32 y_expand_to,s32 w_expand_to,s32 h_expand_to, bool &initialise){
  s32 x_use_end=x_use+w_use;
  s32 y_use_end=y_use+h_use;
  s32 x_end=x_expand_to+w_expand_to;
  s32 y_end=y_expand_to+h_expand_to;
  if (initialise){
    x_use=x_expand_to;
    y_use=y_expand_to;
    w_use=w_expand_to;
    h_use=h_expand_to;
  }
  initialise=false;
  
  if (x_expand_to<x_use){
    x_use=x_expand_to;
  }
  if (y_expand_to<y_use){
    y_use=y_expand_to;
  }
  if (x_end>x_use_end){
    x_use_end=x_end;
  }
  if (y_end>y_use_end){
    y_use_end=y_end;
  }
  w_use=x_use_end-x_use;
  h_use=y_use_end-y_use;
  //  printf("wh %d %d \n",w_use,h_use);
}


void expand_rectangle(s32 &x_use,s32 &y_use,s32 &w_use,s32 &h_use,s32 x,s32 y,s32 w,s32 h){
  bool dummy=false;
  expand_rectangle(x_use,y_use,w_use,h_use,x,y,w,h,dummy);
}


void window_manager::swap_over_selected_and_deselected_window_controls(int _window_selected){
  window_selected=_window_selected;
  int n=(int)window_stack.size();
  for(int i=n-1;i>=1;i--){
    window_stack[i]->is_selected=i==window_selected;
    window_stack[i]->draw_title_bar();
  }
}


//void window_manager::swap_over_selected_and_deselected_window_controls(int _window_selected){
//  s32 old_window_selected=window_selected;
//  window_selected=_window_selected;
//  int n=(int)window_stack.size();
//  for(int i=n-1;i>=1;i--){
//    window_stack[i]->is_selected=i==window_selected;
//  }
//  window_stack[old_window_selected]->draw_furniture();
//  window_stack[window_selected]->draw_furniture();
//}


//void window_manager::make_combos_topmost(){
//  int n=(int)window_stack.size();
//  int index_of_first_normal_window=-1;
//  int found_normal=false;
//  int i=n-1;
//  while(i>0){
//    rtt_window &w=*window_stack[i];
//    if (w.window_type==WINDOW_DISPLAY_TYPE_NORMAL_CLIENT){
//      found_normal=true;
//    }
//    if (found_normal && w.window_type==WINDOW_DISPLAY_TYPE_COMBO_BOX){
//      bring_window_to_front(i);
//      i=n;
//      found_normal=false;
//    }
//    i--;
//  }
//}


int window_manager::bring_window_to_front(int i){
  s32 topmost_window_index=window_stack.size()-1;
  if (i!=topmost_window_index){
    unique_ptr<rtt_window> w=move(window_stack[i]);
    window_stack.erase(window_stack.begin()+i);
    window_stack.push_back(move(w));
    swap_over_selected_and_deselected_window_controls(topmost_window_index);
    //    update_all_windows_contents();
    //  s32 old_x=
    //  void window_manager::determine_region_to_redraw_after_window_motion(u32 moved_window_index,s32 old_x,s32 old_y,s32 old_w,s32 old_h){

    re_blit_window_to_screen(topmost_window_index);
  }
  return topmost_window_index;
}


void window_manager::send_window_to_back(int i){
  int n=(int)window_stack.size();
  unique_ptr<rtt_window> w=move(window_stack[i]);
  window_stack.erase(window_stack.begin()+i);
  window_stack.insert(window_stack.begin()+1,move(w));
  swap_over_selected_and_deselected_window_controls(n-1);
  update_all_windows_contents();
}


void window_manager::check_window_movement_resize_and_default_buttons(int x,int y,bool only_task_bar){
  int n=(int)window_stack.size();
  int en=n-1;
  if (only_task_bar){
    en=0;
  }
  for(int i=en;i>=0;i--){
    rtt_window &w=*window_stack[i];
    if (i>0){
      if (w.check_inside_title_bar(last_mouse_pos)){
        int button=w.check_inside_buttons(last_mouse_pos);
        s32 old_x=w.x;
        s32 old_y=w.y;
        s32 old_w=w.w;
        s32 old_h=w.h;
        if (button==WINDOW_BUTTON_MINIMIZE){
          if (w.window_type==WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST ||w.window_type==WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST){
            break;
          }
          w.minimize();
          determine_region_to_redraw_after_window_motion(i, old_x, old_y, old_w, old_h);
          break;
        }else if(button==WINDOW_BUTTON_MAXIMIZE){
          if (w.window_type==WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST || w.window_type==WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST){
            break;
          }
          w.maximize();
          w.make_bitmap();
          //          w.draw_furniture();
          //          update_all_windows_contents();
          w.paint();
          w.draw_furniture();
          //          if (!w.is_maximized){
            // it sharnk so need to refresh old area
          if (w.w>old_w || w.h>old_h){
            old_x=w.x;
            old_y=w.y;
            old_w=w.w;
            old_h=w.h;
          }            
          //          expand_rectangle(old_x, old_y, old_w, old_h, w.x, w.y, w.w, w.h);
          determine_region_to_redraw_after_window_motion(i, old_x, old_y, old_w, old_h);
            //          }
          //          w.invalidate();
          break;
        }else if(button==WINDOW_BUTTON_CLOSE){
          if (w.window_type==WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST || w.window_type==WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST){
            break;
          }
          //              w.close();
          break;
        }else if (w.check_inside_title_bar(last_mouse_pos)){
          if (w.window_type==WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST || w.window_type==WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST){
            break;
          }
          window_index_move_window=bring_window_to_front(i);
          moving_window_initial_pos=pt(w.x,w.y);
//          window_index_move_window=n-1;
          break;
        }
      }else if (w.scrollbar_h.inside(w,last_mouse_pos)){
        window_index_grabbed_horizontal_scrollbar=i;
        w.scrollbar_h.display_offset_start=w.horizontal_display_offset;
        break;
      }else if (w.scrollbar_v.inside(w,last_mouse_pos)){
        window_index_grabbed_vertical_scrollbar=i;
        w.scrollbar_v.display_offset_start=w.vertical_display_offset;
        break;
      }else if (w.check_inside_window(last_mouse_pos)){
        bring_window_to_front(i);
        break;
      }
    }
    bool need_break=false;
    if (w.check_inside_right_border(last_mouse_pos)){
      window_index_resize_window_right_border=i;
      need_break=true;
    }else if (w.check_inside_left_border(last_mouse_pos)){
      if (i>0){
        window_index_resize_window_left_border=i;
        need_break=true;
      }
    }
    if (i>0){
      if (w.check_inside_bottom_border(last_mouse_pos)){
        window_index_resize_window_bottom_border=i;
        need_break=true;
      }else if (w.check_inside_top_border(last_mouse_pos)){
        window_index_resize_window_top_border=i;
        need_break=true;
      }
    }
    if (need_break){
      moving_window_initial_pos=pt(w.x,w.y);
      resizing_window_initial_w=w.w;
      resizing_window_initial_h=w.h;
      w.is_maximized=false;
      break;
    }
  }
}


void window_manager::mouse_wheel(int dir,int x,int y){
}


void window_manager::mouse_move(int x,int y){
  //  return;
  //  printf("move %d %d\n",x,y);
  bool mouse_manipulating_a_window=false;
  if (window_index_move_window!=-1){
    // moving a window by holding on with mouse
    rtt_window &w=*window_stack[window_index_move_window];
    mouse_manipulating_a_window=true;
    s32 new_x=x-last_mouse_pos.x+moving_window_initial_pos.x;
    s32 new_y=y-last_mouse_pos.y+moving_window_initial_pos.y;
    if (w.x!=new_x || w.y!=new_y){
      s32 old_x=w.x;
      s32 old_y=w.y;
      s32 old_w=w.w;
      s32 old_h=w.h;
      w.x=new_x;
      w.y=new_y;
      w.remember_size();
      determine_region_to_redraw_after_window_motion(window_index_move_window, old_x, old_y, old_w, old_h);
    }
  }
  s32 x_resize=0;
  s32 y_resize=0;
  s32 w_resize=0;
  s32 h_resize=0;
  s32 window_to_resize=-1;
  s32 window_index_resize=-1;
  bool initialise_resize=true;

  if (window_index_resize_window_right_border!=-1){
    // resizing window
    mouse_manipulating_a_window=true;
    if (x-last_mouse_pos.x!=0){
      rtt_window &w=*window_stack[window_index_resize_window_right_border];
      s32 old_w=w.w;
      w.w=x-last_mouse_pos.x+resizing_window_initial_w;
      window_index_resize=window_index_resize_window_right_border;
      window_to_resize=window_index_resize_window_right_border;
      expand_rectangle(x_resize,y_resize,w_resize,h_resize, w.x, w.y, old_w, w.h, initialise_resize);
    }
  }
  if (window_index_resize_window_left_border!=-1){
    // resizing window
    mouse_manipulating_a_window=true;
    if (x-last_mouse_pos.x!=0){
      rtt_window &w=*window_stack[window_index_resize_window_left_border];
      s32 old_x=w.x;
      s32 old_w=w.w;
      int dx=last_mouse_pos.x-x;
      w.w=resizing_window_initial_w+dx;
      w.x=moving_window_initial_pos.x-dx;
      window_index_resize=window_index_resize_window_left_border;
      window_to_resize=window_index_resize_window_left_border;
      expand_rectangle(x_resize,y_resize,w_resize,h_resize, old_x, w.y, old_w, w.h, initialise_resize);
    }
  }
  if (window_index_resize_window_bottom_border!=-1){
    // resizing window
    mouse_manipulating_a_window=true;
    if (y-last_mouse_pos.y!=0){
      rtt_window &w=*window_stack[window_index_resize_window_bottom_border];
      s32 old_h=w.h;
      int dy=last_mouse_pos.y-y;
      w.h=resizing_window_initial_h+dy;
      window_index_resize=window_index_resize_window_bottom_border;
      window_to_resize=window_index_resize_window_bottom_border;
      expand_rectangle(x_resize,y_resize,w_resize,h_resize, w.x, w.y, w.w, old_h, initialise_resize);
    }
  }
  if (window_index_resize_window_top_border!=-1){
    // resizing window
    mouse_manipulating_a_window=true;
    if (y-last_mouse_pos.y!=0){
      rtt_window &w=*window_stack[window_index_resize_window_top_border];
      s32 old_y=w.y;
      s32 old_h=w.h;
      int dy=last_mouse_pos.y-y;
      w.h=resizing_window_initial_h-dy;
      w.y=moving_window_initial_pos.y-dy;
      window_index_resize=window_index_resize_window_top_border;
      window_to_resize=window_index_resize_window_top_border;
      expand_rectangle(x_resize,y_resize,w_resize,h_resize, w.x, old_y, w.w, old_h, initialise_resize);
    }
  }
  if (window_to_resize!=-1){
    rtt_window &w=*window_stack[window_to_resize];
    assert(window_index_resize!=-1);
    w.make_bitmap(true);
    w.remember_size();
    w.paint();
    determine_region_to_redraw_after_window_motion(window_index_resize, x_resize, y_resize, w_resize, h_resize);
    printf("resized %d %d\n",w_resize,h_resize);
  }
  if (window_index_grabbed_horizontal_scrollbar!=-1){
    mouse_manipulating_a_window=true;
    int dx=x-last_mouse_pos.x;
    if (dx!=0){
      rtt_window &w=*window_stack[window_index_grabbed_horizontal_scrollbar];
      //      int client_width_left_to_scroll=w.scrollbar_h.client_total_length_pixels-w.window_display_width_pixels;
      //      w.horizontal_display_offset=w.scrollbar_h.display_offset_start+(int)((float)dx/(float)w.scrollbar_h.grab_bar_pixels_left_to_scroll*(float)client_width_left_to_scroll+0.5f);
      w.horizontal_display_offset= w.scrollbar_h.update_scroll_pos(dx,w.window_display_width_pixels);
      if (w.horizontal_display_offset<0){
        w.horizontal_display_offset=0;
      }
      //      w.draw_furniture();
      w.remember_size();
      //      w.update_furniture_but_dont_repaint_contents();
      w.paint();
      //      update_all_windows_contents();
      w.invalidate();
    }
  }
  if (window_index_grabbed_vertical_scrollbar!=-1){
    mouse_manipulating_a_window=true;
    int dy=y-last_mouse_pos.y;
    if (dy!=0){
      rtt_window &w=*window_stack[window_index_grabbed_vertical_scrollbar];
      //      int client_width_left_to_scroll=w.scrollbar_h.client_total_length_pixels-w.window_display_width_pixels;
      //      w.horizontal_display_offset=w.scrollbar_h.display_offset_start+(int)((float)dx/(float)w.scrollbar_h.grab_bar_pixels_left_to_scroll*(float)client_width_left_to_scroll+0.5f);
      w.vertical_display_offset=w.scrollbar_v.update_scroll_pos(-dy,w.window_display_height_pixels);
      if (w.vertical_display_offset<0){
        w.vertical_display_offset=0;
      }
      //      w.draw_furniture();
      w.remember_size();
      w.paint();
      w.invalidate();
      //      update_all_windows_contents();
    }
  }
  

#define TASK_BAR_SHOW_X_MARGIN 20
  if (!mouse_manipulating_a_window){
    // check task bar and other functions if not doing things to a window
    if (!showing_task_bar){
      if (x<main_x+TASK_BAR_SHOW_X_MARGIN){
        showing_task_bar=true;
        window_stack[0]->x=0;
        window_stack[0]->y=main_h;
        window_stack[0]->invalidate();  // just plot it cos its appeared & its always topmost
        //        window_stack[0]->render_exposed_screen_rectangle(x,y,w,h); // always render window_stack[0] ontop
      }else{
        // send to all windows for now
        int n=(int)window_stack.size();
        for(int i=1;i<n;i++){
          rtt_window &w=*window_stack[i];
          switch(w.window_type){
            case WINDOW_DISPLAY_TYPE_NORMAL_CLIENT:
              w.mouse_move(x,y);
              break;
          }
        }
      }
    }else{
      if (x>main_x+window_stack[0]->w){
        showing_task_bar=false;
        s32 old_x=window_stack[0]->x;
        window_stack[0]->x=main_x-window_stack[0]->w;
        //        backdrop->blit_entire_bitmap_to_screen(backdrop->x,backdrop->y,main_h);
        //        render_all_windows_to_the_screen();
        s32 real_y=window_stack[0]->y-window_stack[0]->h;
        wm_redraw_exposed_screen_rectangle(old_x,real_y,window_stack[0]->w,window_stack[0]->h); // redraw where taskbar was
      }
      render_window(0); // redraw taskbar constantly
    }
    if (mouse_right_button_is_down){
      // check sliding over task bar for previews
      check_task_bar_window_minimize(x,y,true);
    }
    if (mouse_left_button_is_down){
      // check sliding over task bar for previews
      check_task_bar_window_minimize(x,y,false);
    }
  }
  //  // check combo boxes
  //  int n=(int)window_stack.size();
  //  bool redraw=false;
  //  for(int i=1;i<n;i++){
  //    rtt_window &w=*window_stack[i];
  //    if (w.window_type==WINDOW_DISPLAY_TYPE_COMBO_BOX){
  //      redraw|=w.check_combo_mouse_posn(x,y);
  //    }
  //  }
  //  if (redraw){
  //    //    update_all_windows_contents();
  //  }
}


void window_manager::mouse_click(int butt,int x,int y){
  bool check_task_bar=false;
  switch(butt){
    case MOUSE_LEFT_DOUBLE_CLICK:
      {
        check_task_bar_window_minimize(x,y,true);
      }
      check_task_bar=true;
      break;
    case MOUSE_LEFT_DOWN:
      {
        last_mouse_pos=pt(x,y);  // main window now has y increasing up the screen
        mouse_left_button_is_down=true;
        s32 index=check_task_bar_pos(x,y);
        check_window_movement_resize_and_default_buttons(x,y,index==ONLY_OVER_BLANK_BIT_OF_TASK_BAR_INDEX);
        if (index>=0 && index<(s32)window_stack.size()){
          rtt_window &w=*window_stack[index];
          if (w.is_minimized){
            w.un_minimize();
            wm_redraw_exposed_screen_rectangle(w);
            //            s32 real_y=w.y-w.h;
          }
        }
        // check combo boxes/multiple select boxes
        int n=(int)window_stack.size();
        for(int i=1;i<n;i++){
          rtt_window &w=*window_stack[i];
          switch(w.window_type){
            case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST:
            case WINDOW_DISPLAY_TYPE_COMBO_BOX:
              if (w.check_combo_mouse_posn(x,y,MOUSE_LEFT_DOWN)){
                w.paint();
                wm_redraw_exposed_screen_rectangle(w);
              }
              break;
          }
        }
        // check text selection edit box dropdowns
        n=(int)window_stack.size();
        printf("mouse click down n=%d\n",n);
        for(int i=1;i<n;i++){
          bool handled=false;
          rtt_window &w=*window_stack[i];
          //          printf("stack %d=%16x - %16x RTTXCB=%16x\n",i,(uint64_t)window_stack[i].get(),(uint64_t)addressof(window_stack[i]),window_stack[i]->pixels);
          //          printf("stack %d=%16x - %16x RTTXCB=%16x\n",i,(uint64_t)window_stack[i].get(),(uint64_t)addressof(window_stack[i]),window_stack[i]->pixels);
          //          for(auto it=w.text_edit_boxes.begin();it!=w.text_edit_boxes.end();it++){
          //            RTT_text_edit_box &t=**it;
          //            if (t.drop_down_window){
          //              printf("mouse click down n=%d %d\n",n,(int)window_stack.size());
          //              t.current_text=w.items[w.selected_item].text;
          //              s32 old_x=w.x;
          //              s32 old_y=w.y;
          //              s32 old_w=w.w;
          //              s32 old_h=w.h;
          //              window_index_move_window=-1;
          //              printf("deleted t.delete_old_dropdown_selection_window()\n");
          //              t.delete_old_dropdown_selection_window();
          //              t.parent_window.paint();
          //              wm_redraw_exposed_screen_rectangle(old_x,old_y-old_h,old_w,old_h);
          //              wm_redraw_exposed_screen_rectangle(t.parent_window);
          //              break;
          //            }
          //          }
          switch(w.window_type){
            case WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST:
              if (w.check_combo_mouse_posn(x,y,MOUSE_LEFT_DOWN)){
                rtt_text_edit_box &t=*w.text_edit_box_spawned_from;
                printf("mouse click down n=%d %d\n",n,(int)window_stack.size());
                t.current_text=w.items[w.selected_item].text;
                s32 old_x=w.x;
                s32 old_y=w.y;
                s32 old_w=w.w;
                s32 old_h=w.h;
                window_index_move_window=-1;
                printf("deleted t.delete_old_dropdown_selection_window()\n");
                t.delete_old_dropdown_selection_window();
                t.paint();
                t.parent_window.draw_text_edit_box(t,true);
                //                t.parent_window.paint();
                wm_redraw_exposed_screen_rectangle(old_x,old_y-old_h,old_w,old_h);
                wm_redraw_exposed_screen_rectangle(t.parent_window);
                handled=true;
              }
              break;
          }
          if (handled){
            break;
          }
        }
        check_task_bar=true;
      }
      break;
    case MOUSE_LEFT_UP:
      window_index_move_window=-1;
      mouse_left_button_is_down=false;
      window_index_resize_window_right_border=-1;
      window_index_resize_window_left_border=-1;
      window_index_resize_window_bottom_border=-1;
      window_index_resize_window_top_border=-1;
      window_index_grabbed_horizontal_scrollbar=-1;
      window_index_grabbed_vertical_scrollbar=-1;
      check_task_bar=true;
      break;
    case MOUSE_MIDDLE_DOWN:
      break;
    case MOUSE_MIDDLE_UP:
      break;
    case MOUSE_RIGHT_DOWN:
      {
        mouse_right_button_is_down=true;
        last_mouse_pos=pt(x,y);  // main window now has y increasing up the screen
        if (!check_task_bar_window_minimize(x,y,true)){
          // task bar is not selected
          int n=(int)window_stack.size();
          for(int i=n-1;i>=1;i--){
            rtt_window &w=*window_stack[i];
            if (w.check_inside_title_bar(last_mouse_pos)){
              int button=w.check_inside_buttons(last_mouse_pos);
              if (button==WINDOW_BUTTON_MINIMIZE){
                send_window_to_back(i);
                break;
              }else if(button==WINDOW_BUTTON_MAXIMIZE){
                w.maximize();
                w.make_bitmap();
                w.draw_furniture();
                update_all_windows_contents();
                break;
              }else if(button==WINDOW_BUTTON_CLOSE){
                //              w.close();
                break;
              }else if (w.check_inside_title_bar(last_mouse_pos)){
                moving_window_initial_pos=pt(w.x,w.y);
                window_index_move_window=i;
                break;
              }
            }else if (w.scrollbar_h.inside(w,last_mouse_pos) || w.scrollbar_v.inside(w,last_mouse_pos)){
              window_index_grabbed_horizontal_scrollbar=i;
              w.scrollbar_h.display_offset_start=w.horizontal_display_offset;
              window_index_grabbed_vertical_scrollbar=i;
              w.scrollbar_v.display_offset_start=w.vertical_display_offset;
              break;
            }
          }
        }
        // check combo boxes/multiple select boxes
        int n=(int)window_stack.size();
        for(int i=1;i<n;i++){
          rtt_window &w=*window_stack[i];
          switch(w.window_type){
            case WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST:
            case WINDOW_DISPLAY_TYPE_COMBO_BOX:
              if (w.check_combo_mouse_posn(x,y,MOUSE_RIGHT_DOWN)){
                w.paint();
                wm_redraw_exposed_screen_rectangle(w);
              }
              break;
          }
        }
        check_task_bar=true;
      }
      break;
    case MOUSE_RIGHT_UP:
      mouse_right_button_is_down=false;
      window_index_grabbed_horizontal_scrollbar=-1;
      window_index_grabbed_vertical_scrollbar=-1;
      if (mouse_left_button_is_down){
        // swap over control to left button action if moving
        if (window_index_move_window!=-1){
          rtt_window &w=*window_stack[window_index_move_window];
          moving_window_initial_pos=pt(w.x,w.y);
        }
      }else{
        window_index_move_window=-1;
      }
      check_task_bar=true;
      break;
    case MOUSE_WHEEL_DOWN:
      break;
    case MOUSE_WHEEL_UP:
      break;
  }
  if (check_task_bar && showing_task_bar){
    // redraw taskbar if clicked & showing it
    render_window(0);
  }

  // pass wo topmost window
  s32 en=window_stack.size()-1;
  rtt_window &w=*window_stack[en];
  //  rtt_window *w2=window_stack.back();
  //  assert(w==w2);
  //    if (i>0){
  //      if (w.check_inside_title_bar(last_mouse_pos)){
  //        int button=w.check_inside_buttons(last_mouse_pos);
  //        s32 old_x=w.x;
  w.mouse_click_in_window_area(butt,x,y);
}


void window_manager::key(int dir,u32 OS_keycode,bool is_ascii,u8 ASCII){
  //  RTT_key(dir,my_key);
  // decide where keys events go to
  window_stack.back()->key(dir,OS_keycode,is_ascii,ASCII);
  switch(OS_keycode){
    case RTT_XK_Shift_L:
    case RTT_XK_Shift_R:
      key_shift_pressed=dir==KEYPRESS_DOWN?true:false;
      break;
    case RTT_XK_Control_L:
    case RTT_XK_Control_R:
      key_control_pressed=dir==KEYPRESS_DOWN?true:false;
      break;
    case RTT_XK_Alt_L:
    case RTT_XK_Alt_R:
      key_alt_pressed=dir==KEYPRESS_DOWN?true:false;
      break;
  }
}


RTTXCB* window_manager::load_bitmap(string filename){
  return new RTTXCB(device_context,filename.data());
}


int window_manager::load_24bit_BMP_bitmap(const char * filename){
  RTTXCB *b=new RTTXCB(device_context,filename);
  bitmaps.push_back(b);
  return (int)bitmaps.size()-1;
}


void window_manager::load_window_bitmaps(){
  bitmap_window_bottom_border_deselected   =load_24bit_BMP_bitmap("bitmaps/window_bottom_border_deselected.bmp"  );  
  bitmap_window_bottom_border_selected     =load_24bit_BMP_bitmap("bitmaps/window_bottom_border_selected.bmp"    );    
  bitmap_window_left_border_deselected     =load_24bit_BMP_bitmap("bitmaps/window_left_border_deselected.bmp"    );    
  bitmap_window_left_border_selected       =load_24bit_BMP_bitmap("bitmaps/window_left_border_selected.bmp"      );      
  bitmap_window_right_border_deselected    =load_24bit_BMP_bitmap("bitmaps/window_right_border_deselected.bmp"   );   
  bitmap_window_right_border_selected      =load_24bit_BMP_bitmap("bitmaps/window_right_border_selected.bmp"     );     
  bitmap_window_title_bar_deselected       =load_24bit_BMP_bitmap("bitmaps/window_title_bar_deselected.bmp"      );      
  bitmap_window_title_bar_selected         =load_24bit_BMP_bitmap("bitmaps/window_title_bar_selected.bmp"        );        
  bitmap_window_buttons_deselected         =load_24bit_BMP_bitmap("bitmaps/window_buttons_deselected.bmp"        );        
  bitmap_window_buttons_selected           =load_24bit_BMP_bitmap("bitmaps/window_buttons_selected.bmp"          );          
  bitmap_button_down_arrow_pressed         =load_24bit_BMP_bitmap("bitmaps/button_down_arrow_pressed.bmp"        );        
  bitmap_button_down_arrow                 =load_24bit_BMP_bitmap("bitmaps/button_down_arrow.bmp"                );                
  bitmap_button_up_arrow                   =load_24bit_BMP_bitmap("bitmaps/button_up_arrow.bmp"                  );                  
  bitmap_button_up_arrow_pressed           =load_24bit_BMP_bitmap("bitmaps/button_up_arrow_pressed.bmp"          );          
  bitmap_task_bar_item                     =load_24bit_BMP_bitmap("bitmaps/task_bar_item_2048_wide.bmp"          );          
  bitmap_task_bar_item_minimized_indicator =load_24bit_BMP_bitmap("bitmaps/task_bar_item_minimized_indicator.bmp");
  bitmap_scrollbar_down                    =load_24bit_BMP_bitmap("bitmaps/scrollbar_down.bmp"                   );                   
  bitmap_scrollbar_horizontal_background   =load_24bit_BMP_bitmap("bitmaps/scrollbar_horizontal_background.bmp"  );  
  bitmap_scrollbar_horizontal_bar          =load_24bit_BMP_bitmap("bitmaps/scrollbar_horizontal_bar.bmp"         );         
  bitmap_scrollbar_horizontal_bar_middle   =load_24bit_BMP_bitmap("bitmaps/scrollbar_horizontal_bar_middle.bmp"  );  
  bitmap_scrollbar_left                    =load_24bit_BMP_bitmap("bitmaps/scrollbar_left.bmp"                   );                   
  bitmap_scrollbar_right                   =load_24bit_BMP_bitmap("bitmaps/scrollbar_right.bmp"                  );                  
  bitmap_scrollbar_up                      =load_24bit_BMP_bitmap("bitmaps/scrollbar_up.bmp"                     );                     
  bitmap_scrollbar_vertical_background     =load_24bit_BMP_bitmap("bitmaps/scrollbar_vertical_background.bmp"    );    
  bitmap_scrollbar_vertical_bar            =load_24bit_BMP_bitmap("bitmaps/scrollbar_vertical_bar.bmp"           );           
  bitmap_scrollbar_vertical_bar_middle     =load_24bit_BMP_bitmap("bitmaps/scrollbar_vertical_bar_middle.bmp"    );    
  bitmap_text_box                          =load_24bit_BMP_bitmap("bitmaps/text_input_box_black.bmp"             );             
  bitmap_button_normal                     =load_24bit_BMP_bitmap("bitmaps/button_pressed_darker.bmp"            );                   
  bitmap_button_pressed                    =load_24bit_BMP_bitmap("bitmaps/button_normal_darker.bmp"             );                    

  window_border_width                      =bitmaps[bitmap_window_left_border_deselected]->w;
  window_title_bar_height                  =bitmaps[bitmap_window_title_bar_selected]->h;
  #define SMALLEST_TITLE_BAR_WIDTH 32
  //    static int *bitmap_window_title_bar_deselected_scaled[NUM_TITLE_BAR_WIDTHS];
  //  for(int i=0;i<NUM_TITLE_BAR_WIDTHS;i++){
  //    bitmap_window_title_bar_deselected_scaled[i]=new RTTXCB(bitmap_window_title_bar_deselected,
  //  }    
}


void window_manager::set_backdrop(RTTXCB *_backdrop){
  backdrop=unique_ptr<rtt_window>(new rtt_window(*this,-1,"background",0,main_h,main_w,main_h,0,0,0,WINDOW_DISPLAY_TYPE_BACKGROUND,_backdrop->w,_backdrop->h));
  backdrop->blit_bitmap_to_workarea_only_no_furniture_offset(_backdrop);
//  backdrop->blit_bitmap_to_workarea_only(_backdrop);
}


bool window_manager::check_task_bar_window_minimize(int x,int y,bool minimize_it){
  int index=check_task_bar_pos(x,y);
  if (index!=-1 && index!=ONLY_OVER_BLANK_BIT_OF_TASK_BAR_INDEX){
    if (index>0 && index<window_stack.size()){
      rtt_window &w=*window_stack[index];
      if (minimize_it){
        if (!w.is_minimized){
          s32 old_x=w.x;
          s32 old_y=w.y;
          s32 old_w=w.w;
          s32 old_h=w.h;
          w.minimize();
          s32 real_y=old_y-old_h;
          wm_redraw_exposed_screen_rectangle(old_x, real_y, old_w, old_h);
        }
      }else{
        if (w.is_minimized){
          w.un_minimize();
          s32 real_y=w.y-w.h;
          wm_redraw_exposed_screen_rectangle(w.x, real_y, w.w, w.h);
        }
      }
    }
    return true;
  }
  rtt_window &task_bar=*window_stack[0];
  if (x>=task_bar.x && x<=task_bar.x+task_bar.w){
    return true;
  }
  return false;
}


//void window_manager::remove_window_from_active_window_stack(unique_ptr<rtt_window> w){
//  for(auto it=window_stack.begin();it!=window_stack.end();it++){
//    if (*it==w){
//      window_stack.erase(it);
//      break;
//    }      
//  }
//  task_bar_paint(*window_stack[0]);
//}


int window_manager::check_task_bar_pos(int x,int y){
  rtt_window &task_bar=*window_stack[0];
  if (x>=task_bar.x && x<=task_bar.x+task_bar.w){
    int ye=task_bar.y-TASK_BAR_WINDOW_LIST_START_Y;
    int dy=bitmaps[bitmap_task_bar_item]->h+TASK_BAR_WINDOW_LIST_ITEM_GAP;
    int index=1+(ye-y)/dy;
    if (index>0 && index<window_stack.size()){
      return task_bar_window_display_order[index-1].window_index;
    }
    return ONLY_OVER_BLANK_BIT_OF_TASK_BAR_INDEX;
  }
  return -1;
}

rtt_font& window_manager::get_font(bool text_bold){
  if (text_bold){
    return tahoma8_bold;
  }
  return tahoma8_normal;
}

#define TASK_BAR_ITEM_TEXT_OFFSETX 21
#define TASK_BAR_ITEM_TEXT_OFFSETY 2
#define TASK_BAR_ITEM_MIMIMIZED_INDICATOR_X_PLOT_OFFSET 3
#define TASK_BAR_ITEM_MIMIMIZED_INDICATOR_Y_PLOT_OFFSET 3
void window_manager::task_bar_paint(rtt_window &w){
  window_manager &wm=w.wm_member_of;
  u32 n=wm.window_stack.size();
  if (n>1){
    wm.task_bar_window_display_order.resize(n-1);
    for(int i=1;i<(int)wm.window_stack.size();i++){
      wm.task_bar_window_display_order[i-1]=window_sort_struct(wm.window_stack[i]->my_sort_key,i);
    }
    sort(wm.task_bar_window_display_order.begin(),wm.task_bar_window_display_order.end());

    int num_wins=(int)wm.window_stack.size();
    int x=TASK_BAR_WINDOW_LIST_X_BORDER_INLAY;
    int width=w.w-2*TASK_BAR_WINDOW_LIST_X_BORDER_INLAY;
    int y=w.y-TASK_BAR_WINDOW_LIST_START_Y;

    int dy=wm.bitmaps[bitmap_task_bar_item]->h+TASK_BAR_WINDOW_LIST_ITEM_GAP;
    for(int j=0;j<num_wins-1;j++){
      int i=wm.task_bar_window_display_order[j].window_index;
      rtt_window &win=*wm.window_stack[i];
      int yp=y-TASK_BAR_ITEM_MIMIMIZED_INDICATOR_Y_PLOT_OFFSET;
      w.plot_raised_taskbar_item(x,y,width);
      get_font(win.is_selected).p(w,x+TASK_BAR_ITEM_TEXT_OFFSETX,width,y-TASK_BAR_ITEM_TEXT_OFFSETY,black_cr,win.title);
      printf("FUCK %s\n",win.title.data());
      if (win.is_minimized){
        RTTXCB *b=wm.bitmaps[bitmap_task_bar_item_minimized_indicator];
        w.blit_bitmap_to_self(b,x+TASK_BAR_ITEM_MIMIMIZED_INDICATOR_X_PLOT_OFFSET,yp,b->w,b->h,0,0);
      }
      y-=dy;
    }
  }
}

static void a(){};

rtt_font::rtt_font(char*data){
  int p=0;
  for(int i=0;i<256;i++){
    rtt_char &c=chars[i];
    int num=data[p++];
    c.width=data[p++];
    c.pixels.resize(num);
    for(int j=0;j<num;j++){
      int x=data[p++];
      int y=data[p++];
      c.pixels[j]=rtt_charpixel(x,y);
    }
  }
}


char rtt_font::dummy[65536];
void rtt_font::p(RTTXCB&context,int x,int xw,int y,COLORREF col,string &str){
  p(context,x,xw,y,col,str.data());
}


void rtt_font::p(RTTXCB&context,int x,int xw,int y,COLORREF col,const char* format,...){
  va_list vlist;
  va_start(vlist, format);
  vsprintf(dummy,format,vlist);
  va_end(vlist);
  int n=(int)strlen(dummy);
  context.setup_font_plotting(col);
  y=context.h-y;
  for(int i=0;i<n;i++){
    rtt_char &r=chars[dummy[i]];
    for(int j=0;j<(int)r.pixels.size();j++){
      rtt_charpixel &p=r.pixels[j];
      int xp=x+p.x;
      if (xp<=xw){
        context.add_font_pixel(xp,y+p.y);
      }
    }
    x+=r.width;
  }
  context.render_font_pixels();
}

#define CARET_COL1 goldenrod_c
#define CARET_COL2 darkorchid4_c
#define CARET_COL_DESELECTED goldenrod4_c
#define CARET_HEIGHT 14

void rtt_font::print_save_posns_and_draw_caret(RTTXCB&context,int x,int xw,int y,vector<s32> &x_coords,s32 caret_pos,s32 caret_pos_selection_end,s32 caret_style,COLORREF col,const char* format,...){
  va_list vlist;
  va_start(vlist, format);
  vsprintf(dummy,format,vlist);
  va_end(vlist);
  int n=(int)strlen(dummy);
  assert(caret_pos<=n);
  y=context.h-y;
  x_coords.clear();
  x_coords.resize(n);
  if (caret_pos>=0){
    // render caret
    s32 x_char_start=x;
    for(int i=0;i<caret_pos;i++){
      rtt_char &r=chars[(u8)dummy[i]];
      x_char_start+=r.width;
    }
    rtt_char &r=chars[(u8)dummy[caret_pos]];
    s32 x_caret_start=x_char_start;
    s32 caret_width=r.width+2;
    if (caret_pos_selection_end!=-1){
      s32 x_char_start_sel_end=x;
      for(int i=0;i<caret_pos_selection_end;i++){
        rtt_char &r=chars[(u8)dummy[i]];
        x_char_start_sel_end+=r.width;
      }
      caret_width=x_char_start_sel_end-x_caret_start;
    }
    s32 x_caret_end=x_caret_start+caret_width;
    if (x_caret_end>=0 && x_caret_start<xw){
      if (x_caret_end>xw){
        caret_width-=x_caret_end-xw;
      }
      switch(caret_style){
        case CARET_STYLE_DESELECTED:
          context.setup_font_plotting(CARET_COL_DESELECTED);
          for(int yy=y-1;yy<y+CARET_HEIGHT;yy++){
            context.add_font_pixel(x_caret_start,yy);
            context.add_font_pixel(x_caret_end,yy);
          }
          for(int xx=x_caret_start;xx<x_caret_end;xx++){
            context.add_font_pixel(xx,y-1);
            context.add_font_pixel(xx,y+CARET_HEIGHT);
          }
          context.render_font_pixels(); 
          break;
        case CARET_STYLE_SOLID_1:
          context.rectangle_fill(x_caret_start, y, caret_width, CARET_HEIGHT, CARET_COL1, CARET_COL1);
          break;
        case CARET_STYLE_SOLID_2:
          context.rectangle_fill(x_caret_start, y, caret_width, CARET_HEIGHT, CARET_COL2, CARET_COL2);
          break;
      }
    }
  }
  context.setup_font_plotting(col);
  for(int i=0;i<n;i++){
    rtt_char &r=chars[(u8)dummy[i]];
    for(int j=0;j<(int)r.pixels.size();j++){
      rtt_charpixel &p=r.pixels[j];
      int xp=x+p.x;
      if (xp<=xw){
        context.add_font_pixel(xp,y+p.y);
      }
    }
    x_coords[i]=x;
    x+=r.width;
  }
  context.render_font_pixels();
}

rtt_font tahoma8_bold(font_tahoma_8pt);
rtt_font tahoma8_normal(font_tahoma_8pt_normal);

void setup_key_translate(){
  //  memset(key_translate,-1,sizeof(key_translate));
  // lots deleted
}


rtt_rect::rtt_rect(s32 _x,s32 _y,s32 _w,s32 _h):x1(_x),y1(_y){
  x2=x1+_w;
  y2=y1+_h;
}


xcoord::xcoord(s32 _x1,s32 _x2):x1(_x1),x2(_x2){
}


void rect_clipper::window_moved_test(){
  calculate_strips_for_a_window_moving_assuming_new_position_is_already_fully_drawn(200,  400,  450,  150,    250,  450,  450,  150);
  rtt_rect new_rect(100, 600, 300, 200);
  add_rect(new_rect);
  rtt_rect rect(100, 475, 650, 115);
  add_rect(rect);
  rtt_rect rect2(300, 500, 150, 50);
  sub_rect(rect2);
  rtt_rect rect3(50, 630, 150, 20);
  sub_rect(rect3);
  rtt_rect rect4t(150, 670, 120, 20);
  sub_rect(rect4t);
  rtt_rect rect4(150, 700, 250, 20);
  sub_rect(rect4);
  rtt_rect rect5(50, 735, 1250, 20);
  sub_rect(rect5);
  rtt_rect rect5u(270, 425, 60, 100);
  sub_rect(rect5u);
}

#define SEE_DEBUG_COORDS 0

void rect_clipper::calculate_strips_for_a_window_moving_assuming_new_position_is_already_fully_drawn(s32 old_x,s32 old_y,s32 old_w,s32 old_h,s32 new_x,s32 new_y,s32 new_w,s32 new_h){
  //  list<s32> k;
  //  k.push_front(1);
  //  k.push_front(2);
  //  k.push_front(3);
  //  k.push_front(4);
  //  k.push_front(5);
  //  k.push_front(6);
  //  s30;
  //    , //  printf("\n");
  //  for(auto it=k.begin();it!=k.end();it++){
  //    printf("entry %d=%d\n",rtt++,*it);
  //  }
  //  printf("\n");

  // overlapping
  //  old_x=200;  old_y=400;  old_w=150;  old_h=150;    new_x=250;  new_y=450;  new_w=150;  new_h=150;
  
  // overlapping
  //  old_x=250;  old_y=400;  old_w=150;  old_h=150;    new_x=200;  new_y=450;  new_w=150;  new_h=150;
  
  // overlapping
  //  new_x=250;  new_y=400;  new_w=150;  new_h=150;    old_x=200;  old_y=450;  old_w=150;  old_h=150;
  
  // non overlapping
  //  old_x=200;  old_y=200;  old_w=150;  old_h=150;    new_x=250;  new_y=450;  new_w=150;  new_h=150;

  // make rects for initial & final window positions and correct for window origin being at the top left
  assert(new_h>0);
  assert(new_w>0);
  assert(old_h>0);
  assert(old_w>0);
  rtt_rect old_rect(old_x,old_y-old_h,old_w,old_h);
  rtt_rect new_rect(new_x,new_y-new_h,new_w,new_h);
  add_rect(old_rect);
  sub_rect(new_rect);
}


list<rtt_strip>::iterator rect_clipper::insert_strip_ycut(s32 y){
  rtt_strip last_strip(y);
  if (strips.size()==0){
    strips.push_back(rtt_strip(y));
    return strips.begin();
  }
  for(auto it=strips.begin();it!=strips.end();it++){
    s32 strip_y=it->y;
    if (y<strip_y){
      // we're potentially overlapping in Y so duplicate the last strip & boolean add the  the X coordinates
      rtt_strip new_strip=last_strip;
      new_strip.y=y;
      return strips.insert(it,new_strip);
    }else if (y==strip_y){
      // coincident edge
      return it;
    }
    last_strip=*it;
  }
  // off the top
  strips.push_back(rtt_strip(y));
  auto ret=strips.end();
  ret--;
  return ret;
}


void rect_clipper::add_rect(rtt_rect &r){
  s32 ys=r.y1;
  s32 ye=r.y2;
  assert(ys<ye);
#if SEE_DEBUG_COORDS==1
  printf("bool add rect  %d %d %d %d\n",r.x1,r.y1,r.x2,r.y2);
#endif
  auto it_start=insert_strip_ycut(ys);
  auto it_end  =insert_strip_ycut(ye);
  // update x extents of each strip between the insertion points
  assert(it_start!=it_end);
  while(it_start!=it_end){
    it_start->boolean_add_x_coordinates(r.x1,r.x2);
    it_start++;
  }
}


void rect_clipper::sub_rect(rtt_rect &r){
  s32 ys=r.y1;
  s32 ye=r.y2;
  assert(ys<ye);
#if SEE_DEBUG_COORDS==1
  printf("bool sub rectangle %d %d %d %d\n",r.x1,r.y1,r.x2,r.y2);
#endif
  auto it_start=insert_strip_ycut(ys);
  auto it_end  =insert_strip_ycut(ye);
  // update x extents of each strip between the insertion points
  assert(it_start!=it_end);
  while(it_start!=it_end){
    it_start->boolean_sub_x_coordinates(r.x1,r.x2);
    it_start++;
  }
}


void rect_clipper::draw_debug_rect_strips(){
  srand(10);
  //  printf("\n-----------------------------------------\n");
  auto it_start=strips.begin();
  auto it_end=it_start;
  it_end++;
  while(it_start!=strips.end()){  
    s32 ys=it_start->y;
    s32 ye=it_end->y;
    //    printf("strip at y=%d to %d\n",ys,ye);
    for(auto xcit=it_start->xcoords.begin();xcit!=it_start->xcoords.end();xcit++){
      //      s32 x=xcit->x1-500;
      s32 x=xcit->x1;
      s32 y=ys;
      s32 w=(xcit->x2-xcit->x1);
      s32 h=(ye-ys);
      //      printf("  %d-%d",xcit->x1,xcit->x2);
      RTTXCB::draw_rectangle_y_at_top_of_screen(x,y,w+1,h,rand_col());
    }
    it_start++;
    it_end++;
    //    printf("\n");
  }
  //  printf("\n-----------------------------------------\n");
}


void rect_clipper::redraw_exposed_screen_rectangle_strips(window_manager &wm){
#if SEE_DEBUG_COORDS==1
  printf("\n-----------------------------------------\n");
#endif
  auto it_start=strips.begin();
  auto it_end=it_start;
  it_end++;
  while(it_start!=strips.end()){  
    s32 ys=it_start->y;
    s32 ye=it_end->y;
    //    printf("strip at y=%d to %d\n",ys,ye);
    for(auto xcit=it_start->xcoords.begin();xcit!=it_start->xcoords.end();xcit++){
      s32 x=xcit->x1;
      s32 y=ys;
      s32 w=(xcit->x2-xcit->x1);
      s32 h=(ye-ys);
      //      printf("  %d-%d",xcit->x1,xcit->x2);
      wm.wm_redraw_exposed_screen_rectangle(x, y, w+1, h);
      //      RTTXCB::draw_rectangle_y_at_top_of_screen(x,y,w,h,rand_col());
    }
    it_start++;
    it_end++;
    //    printf("\n");
  }
  //  printf("\n-----------------------------------------\n");
}

static void b(){};

void rtt_strip::boolean_add_x_coordinates(s32 xs,s32 xe){
  // simple extents test to add
  if (xcoords.size()){
    if (xs<xcoords.back().x1){
      xcoords.back().x1=xs;
    }
    if (xe>xcoords.back().x2){
      xcoords.back().x2=xe;
    }
  }else{
    xcoords.push_back(xcoord(xs,xe));
  }
}


void rtt_strip::boolean_sub_x_coordinates(s32 xs,s32 xe){
  for(auto it=xcoords.begin();it!=xcoords.end();it++){
    s32 span_s=it->x1;
    s32 span_e=it->x2;
    if (span_s>=xs && span_e<=xe){
      // delete fully enclosed span
      it=xcoords.erase(it);
      continue;
    }
    if (span_s<xs && span_e>xs){
      // span crosses start of subtracted region
      if (span_e<=xe){
        // but span doesn't exit the end of it, so clip span to start of subtracted region
        it->x2=xs-1;
        continue;
      }
      // span extends past the end of the subtracted region
      xcoord old=*it;
      // clip the end of the original span
      it->x2=xs-1;
      // insert new span
      it++;
      old.x1=xe+1;
      it=xcoords.insert(it,old);
      continue;
    }
    if (xe<span_s){
      // span is entirely to the right of the subtracted region so do not modify span
      continue;
    }
    if (span_e<xs){
      // span is entirely to left of subtracted region so do not modify span
      continue; 
    }
    assert(span_s<=xe);
    // span crosses over end of subtracted region so clip start
    it->x1=xe;
  }
}


rtt_strip::rtt_strip(s32 _x1,s32 _x2,s32 _y):y(_y){
  xcoords.push_back(xcoord(_x1,_x2));
}


rtt_strip::rtt_strip(s32 _y):y(_y){
}
