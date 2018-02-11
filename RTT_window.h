#ifndef INCLUDED_RTT_WINDOW_HEADER
#define INCLUDED_RTT_WINDOW_HEADER

#include "RTTGDIXCB.h"
#include "RTT_vector.h"
#include <vector>
#include <string>
#include <queue>
#include <list>
#include "RTT_colours.h"
#include "RTT_text_edit_box.h"
#include "RTT_button.h"
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

#define KEYPRESS_DOWN 0
#define KEYPRESS_UP 1

#define EXTRA_WINDOW_BORDER_SIZE_FOR_GRABBING_RESIZE_BORDERS_EASILY 20

#define WINDOW_DISPLAY_TYPE_NORMAL_CLIENT                0
#define WINDOW_DISPLAY_TYPE_PANE_WINDOW                  1
#define WINDOW_DISPLAY_TYPE_COMBO_BOX                    2
#define WINDOW_DISPLAY_TYPE_TEXT_SELECTION_DROPDOWN_LIST 3
#define WINDOW_DISPLAY_TYPE_MULTIPLE_SELECTION_LIST      4
#define WINDOW_DISPLAY_TYPE_BACKGROUND                   65535

#define WINDOW_BUTTON_MINIMIZE 1
#define WINDOW_BUTTON_MAXIMIZE 2
#define WINDOW_BUTTON_CLOSE 3

#define WINDOW_BUTTON_Y_OFFSET 5
#define WINDOW_BUTTON_X_OFFSET 6
#define WINDOW_BUTTON_WIDTH 16

#define WINDOW_ANIMATION_ACTION_TOGGLE_CARET_1 0
#define WINDOW_ANIMATION_ACTION_TOGGLE_CARET_2 1

#define RAISED_ITEM_BORDER_WIDTH 2

#define TASK_BAR_WINDOW_LIST_X_BORDER_INLAY 4
#define TASK_BAR_WINDOW_LIST_START_Y 200
#define TASK_BAR_WINDOW_LIST_ITEM_GAP 2

#define ONLY_OVER_BLANK_BIT_OF_TASK_BAR_INDEX 10000

#define RESIZE_FROM_ORIG_BMP -1
class rtt_window;
class window_manager;
class combo_box_item;

typedef void (*win_cb)(rtt_window&);

#define DEFAULT_SCROLL_BAR_WIDTH 13  // all scrollbar bitmaps are assumed to conform to this
#define MINIMUM_SCROLLBAR_SIZE 6
#define SB_DRAW_HORIZONTAL 0
#define SB_DRAW_VERTICAL 1

class rtt_scrollbar{
public:
  void draw(rtt_window &w,int x,int y,int length,int window_length,int &client_display_offset,int draw_style);
  bool inside(rtt_window &w,pt &pt);
  int update_scroll_pos(int dx,int window_display_width_pixels);
  int display_offset_start;
  int client_total_length_pixels;
  int max_display_offset;
  int grab_bar_xstart;
  int grab_bar_xend;
  int grab_bar_ystart;
  int grab_bar_yend;
  int grab_bar_pixels_left_to_scroll;
  int last_plot_x;
  int last_plot_y;
};

#define TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_NATIVE_WINDOW_TO_WINDOW_MANAGER_COORDINATES 1
#define TRANSFORM_TYPE_TRANSFORM_MULTIPLE_SELECT_BOX_FROM_WINDOW_MANAGER_TO_NATIVE_WINDOW_COORDINATES 2

class rtt_window:public RTTXCB{
 public:
  rtt_window(window_manager &_wm_member_of,int _id,const char*_title,int _x,int _y,int _w,int _h,COLORREF _window_background_col,win_cb _init_fn,win_cb _paint_fn,int window_type,int _client_scrollable_width,int _client_scrollable_height);
  ~rtt_window();
  void draw_furniture();
  //  void paint_controls();
  void draw_controls();
  void draw_text_edit_boxes();
  void draw_buttons();
  void draw_button(rtt_button &b,bool update_window_rect_on_screen_too);
  void draw_text_edit_box(rtt_text_edit_box &t,bool update_window_rect_on_screen_too);
  void draw_multiple_select_box(rtt_window &w,bool update_window_rect_on_screen_too);
  void draw_rttxcb_clipped_to_workarea(s32 x,s32 y,s32 w,s32 h,RTTXCB &rttxcb,bool update_window_rect_on_screen_too,bool render_with_window_scroll_offset);
  void draw_combo_box_window_furniture();
  void draw_text_selection_dropdown_list_window_furniture();
  void draw_client_window_furniture();
  void draw_pane_window_furniture();
  void transform_multiple_select_window_coordiantes(rtt_window &w_msb,s32 transform_type);
  bool clip_rect_to_window_extents(s32 &rect_x ,s32 &rect_y, s32 &rect_w, s32 &rect_h);
  void render_exposed_screen_rectangle(s32 rect_x ,s32 rect_y, s32 rect_w, s32 rect_h);
  bool check_inside_title_bar(pt &p);
  bool check_inside_window(pt &p);
  bool check_inside_right_border(pt &p);
  bool check_inside_left_border(pt &p);
  bool check_inside_bottom_border(pt &p);
  bool check_inside_top_border(pt &p);
  bool check_inside_horizontal_scroll_bar(pt &p);
  int check_inside_buttons(pt &p);
  void get_bitmaps(RTTXCB*&tb,RTTXCB*&bb,RTTXCB*&wlb,RTTXCB*&wrb,RTTXCB*&wb);
  virtual void paint();
  virtual void rectangle_fill(int x,int y,int w,int h,COLORREF cr,COLORREF cr_fill);
  void update_furniture_but_dont_repaint_contents();
  bool check_combo_mouse_posn(int xt,int yt,u32 mouse_button);
  void paint_as_combo_box(bool title_bar_present=true);
  void plot_vertically_stretched_button_style_bitmap_with_end_caps(RTTXCB *b_middle,RTTXCB *b_caps,int x,int y,int h,int border_h);
  void plot_horizontally_stretched_button_style_bitmap_with_end_caps(RTTXCB *b_middle,RTTXCB *b_caps,int x,int y,int w,int border_w);
  void key(u32 key,u32 OS_keycode,bool is_ascii,u8 ASCII);
  void draw_title_bar();
  bool draw_client_title_bar();
  void draw_pane_title_bar();
  bool draw_combo_box_title_bar();
  void draw_multiple_select_boxes();
  void draw_panes();
  void draw_pane(rtt_pane &b,bool update_window_rect_on_screen_too);
  void plot_raised_taskbar_item(int x,int y,int w);
  void minimize();
  void invalidate();
  void remember_size();
  void maximize();
  void un_maximize();
  void un_minimize();
  void add_scrollbars(int _client_scrollable_width,int _client_scrollable_height);
  void handle_window_animated_elements(s32 action);
  s32 visible_workarea_width();
  s32 visible_workarea_height();
  s32 visible_workarea_end_x();
  s32 visible_workarea_end_y();
  bool operator<(const rtt_window&w);
  virtual void blit_bitmap_to_workarea_only(RTTXCB *rttgdi_source);// designed to blit only to visible workspace region, so title bars arent messed up etc
  virtual void blit_bitmap_to_workarea_only_no_furniture_offset(RTTXCB *rttgdi_source);
  void mouse_click_in_window_area(s32 butt, s32 x_click,s32 y_click);
  void mouse_move(s32 x_mouse,s32 y_mouse);
  bool mouse_click_test_edit_boxes(s32 butt, s32 x_click,s32 y_click);
  bool mouse_click_multiple_select_boxes(s32 butt, s32 x_click,s32 y_click);
  bool mouse_click_buttons(s32 butt, s32 x_click,s32 y_click);
  bool mouse_click_panes(s32 butt, s32 x_click,s32 y_click);
  void add_text_edit_box(string current_text,string name, s32 x,s32 y,s32 w,s32 h,vector<string> entries_to_select,s32 current_selected_index,bool render_with_window_scroll_offset=true);
  void add_button(string current_text,string name, std::function<void(RTTXCB&)> &&click_func,s32 x,s32 y,s32 w,s32 h,bool render_with_window_scroll_offset=true);
  rtt_pane& add_pane(string name,paintfn_type &&_paint_fn,clickfn_type &&_click_fn,mouse_move_fn_type &&_mouse_move_fn, s32 x,s32 y,s32 w,s32 h,bool render_with_window_scroll_offset=true);
  u32 add_knob2(string name, 
                s32 x,
                s32 y,
                s32 _start_frame,
                float _degrees_start,
                float _degrees_end,
                string _group,
                string _desc,
                bool _dont_reload_me);
  u32 add_switch2(string name, 
                  s32 x,
                  s32 y,
                  string _group,
                  string _desc,
                  bool is_momentary,
                  bool _is_flashing,
                  bool _dont_reload_me);
  int horizontal_display_offset;
  int vertical_display_offset;
  rtt_scrollbar scrollbar_h;
  rtt_scrollbar scrollbar_v;
  int window_display_width_pixels;
  int window_display_height_pixels;
  s32 currently_selected_text_edit_box;
  static int sort_index;
  win_cb init_fn;
  win_cb paint_fn;
  int my_sort_key;
  int window_type;
  int x;
  int y;
  bool is_selected;  // am i selected
  bool last_is_selected;  // to determiine when to redraw the titlebar on the screen when focus cahnges
  bool is_minimized;
  bool is_maximized;
  int pre_maximinimize_x;
  int pre_maximinimize_y;
  int pre_maximinimize_w;
  int pre_maximinimize_h;
  string title;
  u32 last_title_bar_width_deselected;
  u32 last_title_bar_width_selected;
  RTTXCB *title_bar_selected;
  RTTXCB *title_bar_deselected;
  bool grabbed_h_scrollbar; // if this window is embedded in anything then this indicates its scrollbar is grabbed
  bool grabbed_v_scrollbar; // if this window is embedded in anything then this indicates its scrollbar is grabbed
  window_manager &wm_member_of;
  // combo box data
  void new_item(int index,string &text);
  void add_combo_items(vector<string> &items);
  vector<combo_box_item> items;
  int selected_item;
  int selected_item_alt;
  list<int> selected_items;  // if it's got multiple selections this list contains the indices of the selected items
  s32 flash_my_carrot;
  // text edit boxes
  vector<unique_ptr<rtt_button>> buttons;
  vector<unique_ptr<rtt_text_edit_box>> text_edit_boxes;
  vector<unique_ptr<rtt_window>> multiple_select_boxes;
  vector<unique_ptr<rtt_pane>> panes;
  void add_multiple_select_box(s32 x,s32 y,s32 w, s32 h, vector<string> &entries);
  rtt_text_edit_box *text_edit_box_spawned_from;
  COLORREF col_combo_box_single_selection_rect_background;
  COLORREF col_combo_box_single_selection_rect_border;
  COLORREF col_combo_box_single_selection_font;
  COLORREF col_combo_box_multiple_selection_rect_background;
  COLORREF col_combo_box_multiple_selection_rect_border;
  COLORREF col_combo_box_multiple_selection_font;
  COLORREF default_font_col;
  pt last_mouse_pos;
};


#endif
