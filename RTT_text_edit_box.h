#ifndef INCLUDED_RTT_TEXT_EDIT_BOX
#define INCLUDED_RTT_TEXT_EDIT_BOX

#include <vector>
#include <string>
#include <queue>
#include <list>
#include <memory>

#include "RTT_colours.h"

using namespace std;
class rtt_window;
class rtt_font;

//#define RTT_EDIT_BOX_EDITING_STATE_ACTION_SET_FROM_SUPPLIED_X_COORDINATE 0
//#define RTT_EDIT_BOX_EDITING_STATE_ACTION_GET_X_COORDINATE_FROM_STRING_EDITING 1
#define RTT_EDIT_BOX_EDITING_STATE_OFF               0 // display just the font
#define RTT_EDIT_BOX_EDITING_STATE_SHOW_CARET_GREYED 1 // indicate the caret position by drawing greyed caret
#define RTT_EDIT_BOX_EDITING_STATE_EDITING           2 // display full caret and monitor input to reposition caret & edit text

#define RTT_EDIT_BOX_MODE_SINGLE_SELECT_THEN_CLOSE 0
#define RTT_EDIT_BOX_MODE_MULTIPLE_SELECT_STAY_OPEN 1

#define RTT_TEXT_EDIT_BOX_RETURN_ACTION_NOTHING          0
#define RTT_TEXT_EDIT_BOX_RETURN_ACTION_JUST_REPAINT     1
#define RTT_TEXT_EDIT_BOX_RETURN_ACTION_FINISHED         2
#define RTT_TEXT_EDIT_BOX_RETURN_ACTION_NEXT_EDITBOX     3
#define RTT_TEXT_EDIT_BOX_RETURN_ACTION_PREVIOUS_EDITBOX 4

class rtt_text_edit_box:public RTTXCB{
 public:
  rtt_text_edit_box(rtt_window&_parent_window,
                    string _current_text,
                    string _name, 
                    s32 _x,
                    s32 _y,
                    s32 _w,
                    s32 _h,
                    vector<string> _entries_to_select,
                    s32 _current_selected_index,
                    bool _render_with_window_scroll_offset=true);
  s32 mouse_click(s32 butt,s32 x,s32 y);
  void paint();
  void redraw_text_and_caret(vector<s32> &x_coords,bool caret_style);
  s32 get_available_text_rendering_width();
  u32 key(u32 key_action,u32 OS_keycode,bool is_ascii,u8 ASCII);
  void open_dropdown_selection_window();
  void delete_old_dropdown_selection_window();
  void hide_dropdown_selection_window();
  rtt_font& get_font();
  rtt_window &parent_window;
  string name;
  string current_text;
  vector<string> entries_to_select;
  s32 current_selected_index;
  s32 x; // position relative to parent window
  s32 y; // width & height are in the bitmap
  bool render_with_window_scroll_offset;
  bool text_bold;
  COLORREF text_colour;
  s32 editing_state;
  s32 caret_pos;
  s32 editing_offset;
  s32 caret_style;
  RTTXCB *cached_background;
  bool already_cached_background;
  unique_ptr<rtt_window> drop_down_window;
  //  rtt_window* drop_down_window;
};

void stretch_horizontal_border_bitmap(RTTXCB *bitmap_target, RTTXCB *bitmap_source, s32 border_thickness, s32 source_y_start, s32 target_y_start);
void stretch_horizontal_border(RTTXCB *bitmap_target, RTTXCB *bitmap_source, s32 border_thickness, s32 y_offset);
void horizontally_tile_bitmap_with_border_to_arbitrary_width_bitmap(RTTXCB *bitmap_target,
                                                                    RTTXCB *bitmap_source,
                                                                    s32 border_thickness,
                                                                    s32 source_bitmap_width,
                                                                    s32 destination_width_to_tile);
void copy_end_border_and_bitmap(RTTXCB *bitmap_target,
                                RTTXCB *bitmap_source,
                                s32 border_thickness,
                                s32 source_bitmap_width);
#endif
