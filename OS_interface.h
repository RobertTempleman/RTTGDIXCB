#ifndef INCLUDED_OS_interface_header
#define INCLUDED_OS_interface_header

#include "RTT_config.h"
#include "RTTGDIXCB.h"
#include "RTT_vector.h"
#include <vector>
#include <string>
#include <queue>
#include <list>
#include <memory>
#include <iomanip>
#include <fstream>
#include <assert.h>
#include "RTT_types.h"
#include "RTT_colours.h"
#include "RTT_window.h"
#include "RTT_pane.h"

using namespace std;

typedef unsigned int unt ;
typedef RVEC2<int> pt;


#define MOUSE_LEFT_DOUBLE_CLICK 0
#define MOUSE_LEFT_DOWN 1
#define MOUSE_LEFT_UP 11
#define MOUSE_MIDDLE_DOWN 2
#define MOUSE_MIDDLE_UP 12
#define MOUSE_RIGHT_DOWN 3
#define MOUSE_RIGHT_UP 13
#define MOUSE_WHEEL_UP 100
#define MOUSE_WHEEL_DOWN 101

#define RTT_EDIT_BOX_MOUSE_CLICK_RETURN_NOTHING 0
#define RTT_EDIT_BOX_MOUSE_CLICK_RETURN_CLICKED_BUTTON 1
#define RTT_EDIT_BOX_MOUSE_CLICK_RETURN_CLICKED_IN_TEXT_EDIT_BOX_TEXT_AREA 2

#define RTT_BUTTON_MOUSE_RETURN_NOTHING 0
#define RTT_BUTTON_MOUSE_CLICKED_BUTTON 1
#define RTT_BUTTON_MOUSE_UNCLICKED_BUTTON 2

#define KEYPRESS_DOWN 0
#define KEYPRESS_UP 1

#define EXTRA_WINDOW_BORDER_SIZE_FOR_GRABBING_RESIZE_BORDERS_EASILY 20


#define WINDOW_BUTTON_MINIMIZE 1
#define WINDOW_BUTTON_MAXIMIZE 2
#define WINDOW_BUTTON_CLOSE 3

#define WINDOW_BUTTON_Y_OFFSET 5
#define WINDOW_BUTTON_X_OFFSET 6
#define WINDOW_BUTTON_WIDTH 16

#define RAISED_ITEM_BORDER_WIDTH 2

#define TASK_BAR_WINDOW_LIST_X_BORDER_INLAY 4
#define TASK_BAR_WINDOW_LIST_START_Y 200
#define TASK_BAR_WINDOW_LIST_ITEM_GAP 2

#define ONLY_OVER_BLANK_BIT_OF_TASK_BAR_INDEX 10000

#define CARET_STYLE_DESELECTED 0
#define CARET_STYLE_SOLID_1    1
#define CARET_STYLE_SOLID_2    2

#define RESIZE_FROM_ORIG_BMP -1

class rtt_window;
class window_manager;
typedef void (*win_cb)(rtt_window&);

u32 GetTickCount();

// GDI like wrapper to aid porting GDI code

#define PS_DOT 1
#define PS_SOLID 0

u32 GetTickCount();


class HPEN{
public:
  HPEN(){};
  HPEN(s32 _style,s32 _width,COLORREF _col):style(_style),width(_width),col(_col){}
  s32 style;
  s32 width;
  COLORREF col;
};


class RECT{
 public:
  RECT(){}
  RECT(s32 _left,s32 _right,s32 _top,s32 _bottom):left(_left),right(_right),top(_top),bottom(_bottom){}; 
  s32 left; 
  s32 right; 
  s32 top; 
  s32 bottom; 
};

  
class rtt_charpixel{
public:
  rtt_charpixel(){};
  rtt_charpixel(int _x,int _y):x(_x),y(_y){};
  char x;
  char y;
};


class rtt_char{
 public:
  rtt_char(){
    width=0;
  }
  char width;
  vector<rtt_charpixel> pixels;
};

class rtt_font{
 public:
  rtt_font(char*data);
  static char dummy[65536];
  rtt_char chars[256];
  //  s32 caret_pos;
  //  s32 caret_state;
  void p(RTTXCB&context,int x,int xw,int y,COLORREF col,string &str);
  void p(RTTXCB&context,int x,int xw,int y,COLORREF col,const char* format,...);
  void print_save_posns_and_draw_caret(RTTXCB&context,int x,int xw,int y,vector<s32> &x_coords,s32 caret_pos,s32 caret_pos_selection_end,s32 caret_deselected,COLORREF col,const char* format,...);
};


#define COMBO_BOX_ITEM_SPACING 12
class combo_box_item{
 public:
  combo_box_item(int _index,string _text):index(_index),text(_text){};
  combo_box_item(){};
  int index;
  string text;
};


class rtt_input_box{
  // editable text input box with a dropdown containing selectable items
  // the dropdown will activate a combo box
 public:
  string current_data;
  vector<combo_box_item> items;
};



//#define DEFAULT_SCROLL_BAR_WIDTH 13  // all scrollbar bitmaps are assumed to conform to this
//#define MINIMUM_SCROLLBAR_SIZE 6
//#define SB_DRAW_HORIZONTAL 0
//#define SB_DRAW_VERTICAL 1
//
//class rtt_scrollbar{
//public:
//  void draw(rtt_window*w,int x,int y,int length,int window_length,int &client_display_offset,int draw_style);
//  bool inside(rtt_window *w,pt &pt);
//  int update_scroll_pos(int dx,int window_display_width_pixels);
//  int display_offset_start;
//  int client_total_length_pixels;
//  int max_display_offset;
//  int grab_bar_xstart;
//  int grab_bar_xend;
//  int grab_bar_ystart;
//  int grab_bar_yend;
//  int grab_bar_pixels_left_to_scroll;
//  int last_plot_x;
//  int last_plot_y;
//};


class rtt_text_and_edit_box_with_auto_dropdown:public RTTXCB{
 public:
  rtt_text_and_edit_box_with_auto_dropdown(string &_current_text,vector<string> _entries_to_select,s32 _current_selected_index=-1);
  string current_text;
  vector<string> entries_to_select;
  s32 current_selected_index;
};



class window_sort_struct{
public:
  window_sort_struct(int _sort_key,int _window_index):sort_key(_sort_key),window_index(_window_index){};
  window_sort_struct(){};
  int sort_key;
  int window_index;
  bool operator <(const window_sort_struct &w){return sort_key<w.sort_key;}
};



#define NUM_TITLE_BAR_WIDTHS 8

class window_manager{
public:
  window_manager();
  ~window_manager();
  int main_x;
  int main_y;
  int main_w;
  int main_h;

  static int bitmap_window_bottom_border_deselected;
  static int bitmap_window_bottom_border_selected;
  static int bitmap_window_left_border_deselected;
  static int bitmap_window_left_border_selected;
  static int bitmap_window_right_border_deselected;
  static int bitmap_window_right_border_selected;
  static int bitmap_window_title_bar_deselected;
  //  static int *bitmap_window_title_bar_deselected_scaled[NUM_TITLE_BAR_WIDTHS];
  static int bitmap_window_title_bar_selected;
  //  static int *bitmap_window_title_bar_selected_scaled[NUM_TITLE_BAR_WIDTHS];
  static int bitmap_window_buttons_deselected;
  static int bitmap_window_buttons_selected;
  static int bitmap_task_bar_item;
  static int bitmap_button_down_arrow_pressed;
  static int bitmap_button_down_arrow;
  static int bitmap_button_up_arrow;
  static int bitmap_button_up_arrow_pressed;
  static int bitmap_task_bar_item_minimized_indicator;
  static int bitmap_scrollbar_down;
  static int bitmap_scrollbar_horizontal_background;
  static int bitmap_scrollbar_horizontal_bar;
  static int bitmap_scrollbar_left;
  static int bitmap_scrollbar_right;
  static int bitmap_scrollbar_up;
  static int bitmap_scrollbar_vertical_background;
  static int bitmap_scrollbar_vertical_bar;
  static int bitmap_scrollbar_vertical_bar_middle;
  static int bitmap_scrollbar_horizontal_bar_middle;
  static int bitmap_text_box;
  static int bitmap_button_pressed;
  static int bitmap_button_normal;

  static int window_border_width;
  static int window_title_bar_height;

  int window_selected;
  bool mouse_left_button_is_down;
  bool mouse_right_button_is_down;

  int window_index_move_window;
  int window_index_resize_window_left_border;
  int window_index_resize_window_right_border;
  int window_index_resize_window_bottom_border;
  int window_index_resize_window_top_border;
  int window_index_grabbed_horizontal_scrollbar;
  int window_index_grabbed_vertical_scrollbar;

  pt last_mouse_pos;
  pt moving_window_initial_pos;
  int resizing_window_initial_w;
  int resizing_window_initial_h;
  bool showing_task_bar;
  bool key_shift_pressed;
  bool key_control_pressed;
  bool key_alt_pressed;

  //  vector<unique_ptr<rtt_window>> window_stack;
  vector<unique_ptr<rtt_window>> window_stack;
  vector<RTTXCB*> bitmaps;
  vector<window_sort_struct> task_bar_window_display_order;
  //  unique_ptr<rtt_window> task_bar;
  RTT_DC device_context; // this is per window & X-connection XCB/X

  unique_ptr<rtt_window> backdrop;  // tiled image for the backdrop
  void set_backdrop(RTTXCB *_backdrop);

  rtt_window& new_window(int id,const char*title,int x,int y,int w,int h,COLORREF background_col,win_cb init_fn,win_cb paint_fn,int _window_type,int _client_scrollable_width,int _client_scrollable_height);
  void set_main_window_geometry(int x,int y,int w,int h,bool hide_borders);
  void init(RTT_DC dc);
  void update();
  void update_all_windows_contents();
  void render_all_windows_to_the_screen();
  void wm_redraw_exposed_screen_rectangle(s32 x ,s32 y, s32 w, s32 h);
  void wm_redraw_exposed_screen_rectangle(rtt_window &w);
  void render_window(int i);
  void re_blit_window_to_screen(int i);
  static rtt_font& get_font(bool text_bold);
  void mouse_wheel(int dir,int x,int y);
  void mouse_move(int x,int y);
  void fake_window_move();
  void swap_over_selected_and_deselected_window_controls(int _window_selected);
  void mouse_click(int butt,int x,int y);
  void check_window_movement_resize_and_default_buttons(int x,int y,bool only_task_bar=false);
  bool check_task_bar_window_minimize(int x,int y,bool minimize_it);
  int bring_window_to_frontOLD(int i);
  int bring_window_to_front(int i);
  void send_window_to_back(int i);
  void remove_window_from_active_window_stack(unique_ptr<rtt_window> w);
  int check_task_bar_pos(int x,int y);
  RTTXCB* load_bitmap(string filename);
  int load_24bit_BMP_bitmap(const char *filename);
  void load_window_bitmaps();
  void key(int dir,u32 OS_keycode,bool is_ascii,u8 ASCII);
  static void task_bar_paint(rtt_window &w);
  void make_combos_topmost();
  void determine_region_to_redraw_after_window_motion(u32 moved_window_index,s32 old_x,s32 old_y,s32 old_w,s32 old_h);
  void set_animation_update_function(std::function<void()> func);
};

void setup_key_translate();
void cp(const char* format,...);

extern window_manager wm;
extern rtt_font tahoma8_bold;
extern rtt_font tahoma8_normal;

class rtt_rect{
  // assume x1<=x2 & y1<y2
 public:
  rtt_rect(s32 _x,s32 _y,s32 _w,s32 _h);
  s32 x1;
  s32 y1;
  s32 x2;
  s32 y2;
};


class xcoord{
 public:
  xcoord(s32 _x1,s32 _x2);
  s32 x1;
  s32 x2;
};


class rtt_strip{
  // assume xcoords are sorted _x1<=_x2
 public:
  rtt_strip(s32 _x1,s32 _x2,s32 _y);
  rtt_strip(s32 _y);
  void boolean_add_x_coordinates(s32 xs,s32 xe);
  void boolean_sub_x_coordinates(s32 xs,s32 xe);
  list<xcoord> xcoords;
  s32 y;
};


class rect_clipper{
 public:
  void calculate_strips_for_a_window_moving_assuming_new_position_is_already_fully_drawn(s32 old_x,s32 old_y,s32 old_w,s32 old_h,s32 new_x,s32 new_y,s32 new_w,s32 new_h);
  void window_moved_test();
  void add_rect(rtt_rect &r);
  void add_rectO(rtt_rect &r);
  void sub_rect(rtt_rect &r);
  void draw_debug_rect_strips();
  void redraw_exposed_screen_rectangle_strips(window_manager &wm);
  list<rtt_rect> rects;
  list<rtt_strip> strips;
  list<s32> ycuts;
  list<rtt_strip>::iterator insert_ycut_and_duplicate_existing_ycut(s32 y);
  list<rtt_strip>::iterator insert_strip_ycut(s32 y);
};


extern bool can_print_debug_spam;
#endif
