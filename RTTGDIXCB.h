#pragma once
#include "types.h"
#include <X11/keysymdef.h>
#include "RTT_2.h"
#include "EasyBMP_1.06/EasyBMP_BMP.h"

//enum{
//  DC_PEN=1,
//  DC_BRUSH=2
//};
//
//struct cached_gdi_pen{
//  HPEN pen;
//  COLORREF c;
//  bool valid;
//};
//
//struct cached_gdi_brush{
//  HBRUSH brush;
//  COLORREF c;
//  bool valid;
//};
//
//#define NUM_CACHED_GDI_OBJECTS 1000
//
//class RTTGDI{
//private:
//  static void SelectStockObject2(HDC dc,int obj);
//  static void SetDCPenColor2(HDC dc,COLORREF c);
//  static void SetDCBrushColor2(HDC dc,COLORREF c);
//  static int num_pens;
//  static cached_gdi_pen hpens[NUM_CACHED_GDI_OBJECTS];
//  static int num_brushes;
//  static cached_gdi_brush hbrushes[NUM_CACHED_GDI_OBJECTS];
//
//  HBITMAP bitmap;
//  //  BITMAP *pbitmap; // if this holds a bitmap from a file
//  HDC bitmap_dc;
//  static HDC main_OS_window_dc;
//  static HDC saved_main_OS_window_dc;
//  COLORREF background_col;
//  HPEN pen;
//
//public:
//  RTTGDI(char *filename);
//  RTTGDI(int _w,int _h,COLORREF _background_col);
//
//  int w;
//  int h;
//  static void set_OS_device_context(HDC dc);
//  void blit_entire_bitmap_to_screen(int x,int y,int main_winow_h);
//  void blit_bitmap_to_screen(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y,int main_winow_h);
//  void blit_bitmap_to_self(RTTGDI *rttgdi_source,int screen_x,int screen_y,int screen_w,int screen_h,int copy_from_rectangle_x,int copy_from_rectangle_y);
//  void blit_bitmap_to_self_stretched_to_fit(RTTGDI *rttgdi_source,int screen_x,int screen_y,int screen_w,int screen_h,int copy_from_rectangle_x,int copy_from_rectangle_y,int copy_from_w,int copy_from_h);
//  virtual void blit_bitmap_to_workarea_only(RTTGDI *rttgdi_source){};// designed to blit only to visible workspace region of whatever inherits this, so title bars arent messed up etc
//  void line(int x1,int y1,int x2,int y2,COLORREF cr);
//  void rectangle(int x,int y,int w,int h,COLORREF cr);
//  virtual void rectangle_fill(int x,int y,int w,int h,COLORREF cr,COLORREF cr_fill);
//  void window_lines(int title_bar_h,COLORREF border_col,COLORREF title_bar_col);
//  void make_bitmap();
//  virtual void paint();
//  void clear();
//  void setpixel(int x,int y,COLORREF col);
//  void start_render_to_backbuffer();
//  void swap_back_buffer_to_main();
//  BOOL load_24bit_BMP( char*filename);
//  BOOL load_24bit_BMPO( char*filename);
//};

// a graphics context that the RTTXCB object draws into or creates & renders bitmaps into
class RTT_DC{
 public:
  xcb_connection_t *connection;
  xcb_drawable_t    window;
  xcb_screen_t     *screen;
};

//struct cached_gdi_pen{
//  HPEN pen;
//  COLORREF c;
//  bool valid;
//};
//
//struct cached_gdi_brush{
//  HBRUSH brush;
//  COLORREF c;
//  bool valid;
//};

//struct cache_graphics_context{
//  // for caching font cols etc..
//  COLORREF col;
//  xcb_gcontext_t graphics_context;
//};
//  


#define NUM_CACHED_GDI_OBJECTS 1000
#define MAX_FONT_POINTS_TO_CACHE 256 // cache up this many pts before doing an xcb poly point

#define MAX_NUM_POINTS_PER_POLYLINE_CACHED 65536

#define RTTXCB_COPY_VERBATIM 0
#define RTTXCB_COPY_IMAGE_FROM_SUPPLIED_RTTXCB 1

void OS_console_printf(const char *txt);

class RTTXCB{
  //  friend class unique_ptr;
 public:
  ~RTTXCB();
private:
  //  void SelectStockObject2(HDC dc,int obj);
  //  void SetDCPenColor2(HDC dc,COLORREF c);
  void set_fill_colour(COLORREF c);
  static int num_pens;
  //  static cached_gdi_pen hpens[NUM_CACHED_GDI_OBJECTS];
  static int num_brushes;
  //  static cached_gdi_brush hbrushes[NUM_CACHED_GDI_OBJECTS];
  static s32 main_window_h_static;
  static RTT_DC dc_static;
  static xcb_gcontext_t debug_gc1;
  static xcb_gcontext_t debug_gc2;
  
  xcb_pixmap_t pixmap;
  xcb_image_t *image;
  uint8_t *image32;
  xcb_gcontext_t graphics_context;
  s32 bitmap;
  //  BITMAP *pbitmap; // if this holds a bitmap from a file
  s32 bitmap_dc;
  //  HPEN pen;
  static map<COLORREF,xcb_gcontext_t> cached_graphics_contexts;  
  static map<COLORREF,xcb_gcontext_t> cached_solid_fill_colour_graphics_contexts;  

  RTT_DC device_context;
  RTT_DC saved_main_OS_window_dc;

  BMP bmp;
  bool first_time;
  
  vector<xcb_point_t> cached_pts;

public:
  void check_initialisation_of_debug_gfx();
  RTTXCB(RTT_DC &dc,const char *filename);
  RTTXCB(RTT_DC &dc,int _w,int _h,COLORREF _background_col,const char* _name);
  RTTXCB(RTTXCB &source,int dest_w,int dest_h,int source_x,int source_y,int source_w,int source_h,const char* _name);
  RTTXCB(RTTXCB &rttxcb,u32 action=RTTXCB_COPY_IMAGE_FROM_SUPPLIED_RTTXCB);

  COLORREF background_col;
  COLORREF last_col;
  xcb_point_t points[MAX_NUM_POINTS_PER_POLYLINE_CACHED];
  int w;  // the width requested for generation
  int h;  // the height requested for generation
  int pixmap_w;  // the actual width of the pixmap used for the object
  int pixmap_h;  // the actual height of the pixmap used for the object
  u8 *bmp_file;
  u8 *pixels;
  string name;
  //  const char* name;
  bool font_plotting_initialised;
  xcb_gcontext_t foreground;
  xcb_gcontext_t fill_colour_gc;
  xcb_point_t font_points[MAX_FONT_POINTS_TO_CACHE];
  s32 cur_font_point;
  void select_col(COLORREF col);
  void setup_font_plotting(COLORREF col);
  void add_font_pixel(s16 xp,s16 yp);
  void render_font_pixels();

  void win_set_main_window_geometry(int x,int y,int w,int h,bool hide_borders);
  void set_OS_device_context(RTT_DC &dc);
  void blit_entire_bitmap_to_screen(int x,int y,int main_winow_h);
  void rescale_bitmap(int dest_w,int dest_h,int source_x,int source_y,int source_w,int source_h);
  void blit_bitmap_to_screen(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y,int main_winow_h);
  void blit_bitmap_to_screen_window_mode(int sx,int sy,int sw,int sh,s32 window_x,s32 window_y,int main_winow_h);
  void blit_bitmap_to_screen_debug_rect(int sx,int sy,int sw,int sh,int bitmap_x,int bitmap_y,int main_winow_h);
  void blit_bitmap_to_screen_correct_for_Y_axis_irregularity(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y,int main_winow_h,int sub_window_h);
  void blit_bitmap_rectangle_to_screen_X_window_coords(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y);
  void blit_bitmap_to_self(RTTXCB &rttgdi_source,
                           int dest_x,
                           int dest_y,
                           int width,
                           int height,
                           int source_x,
                           int source_y,
                           bool y_is_up);
  void blit_bitmap_to_self(RTTXCB *rttgdi_source,int screen_x,int screen_y,int screen_w,int screen_h,int copy_from_rectangle_x,int copy_from_rectangle_y,bool y_is_up=true);
  void blit_bitmap_to_self_stretched_to_fit(RTTXCB *rttgdi_prestretched_source,int screen_x,int screen_y,int screen_w,int screen_h,int copy_from_rectangle_x,int copy_from_rectangle_y,int copy_from_w,int copy_from_h);
  virtual void blit_bitmap_to_workarea_only(RTTXCB *rttgdi_source){};// designed to blit only to visible workspace region of whatever inherits this, so title bars arent messed up etc
  virtual void line(s16 x1,s16 y1,s16 x2,s16 y2,COLORREF cr);
  void line_cached_start(s16 x1,s16 y1,COLORREF cr);
  void line_cached_add_pt(s16 x1,s16 y1);
  void line_cached_draw();
  virtual void circle(s16 x,s16 y,u16 r,COLORREF cr);
  virtual void ellipse(s16 x,s16 y,u16 w,u16 h,COLORREF cr,s16 start_ang=0,s16 arc_ang=360<<1);
  virtual void rectangle(int x,int y,int w,int h,COLORREF cr);
  virtual void rectangle_fill(s16 x,s16 y,s16 w,s16 h,COLORREF cr,COLORREF cr_fill);
  void window_lines(int title_bar_h,COLORREF border_col,COLORREF title_bar_col);
  void make_bitmap(bool i_am_a_window_resizing=false);
  virtual void paint();
  void clear();
  void setpixel(s16 x,s16 y,COLORREF col);
  void start_render_to_backbuffer();
  void swap_back_buffer_to_main();
  BOOL load_24bit_BMP_RTT(const char*filename);
  BOOL load_24bit_BMP(const char*filename);
  void RTTXCB_copy_area_checked();
  void resample(int dest_w,int dest_h,int source_x,int source_y,int source_w,int source_h);
  static void draw_rectangle(s32 x1,s32 y1,s32 w,s32 h,COLORREF col);
  static void draw_rectangle_y_at_top_of_screen(s32 x,s32 y,s32 w,s32 h,COLORREF col);
  static pthread_mutex_t xcb_access_gating_mutex;
  s32 id;
};  

extern bool RTTXCB_debug_mode;
extern bool break_now;
