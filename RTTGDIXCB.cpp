// GDI wrapper
//#include "stdafx.h"
#include "RTT_2.h"
#include "OS_interface.h"
#include "RTT_colours.h"
#include <cassert>
#include <pthread.h>

#define DO_FLUSH_AND_CHECK 0
#define DEBUG_PRINT_BLITS 0
#define SEE_DEBUG_CONSTRUCTIONS 0
#define PIXMAP_RESIZING_GRANULARITY 128


int RTTXCB::num_pens=0;
//cached_gdi_pen RTTXCB::hpens[NUM_CACHED_GDI_OBJECTS];
int RTTXCB::num_brushes=0;
//cached_gdi_brush RTTXCB::hbrushes[NUM_CACHED_GDI_OBJECTS];
//HPEN stock_dc_pen;
HBRUSH stock_dc_brush;
s32 RTTXCB::main_window_h_static;
RTT_DC RTTXCB::dc_static;

bool debug_stuff_initialised=false;
xcb_gcontext_t RTTXCB::debug_gc1;
xcb_gcontext_t RTTXCB::debug_gc2;

bool break_now=false;

void OS_console_printf(const char *txt){
  printf(txt);
}

pthread_mutex_t RTTXCB::xcb_access_gating_mutex=PTHREAD_MUTEX_INITIALIZER;

RTTXCB::~RTTXCB(){
  printf("\nRTTXCB::~RTTXCB this=%10x  name=%s\n",(uint64_t)this,name.data());
}


void RTTXCB::win_set_main_window_geometry(int x,int y,int w,int h,bool hide_borders){
}


//void RTTXCB::SelectStockObject2(HDC dc,int obj){
//}
//
//
//void RTTXCB::SetDCPenColor2(HDC dc,COLORREF c){
//}

map<COLORREF,xcb_gcontext_t> RTTXCB::cached_solid_fill_colour_graphics_contexts;  

void RTTXCB::set_fill_colour(COLORREF col){
  if (cached_solid_fill_colour_graphics_contexts.find(col)==cached_solid_fill_colour_graphics_contexts.end()){
    uint32_t values[2];
    uint32_t mask=0;
    fill_colour_gc=xcb_generate_id(device_context.connection);
    mask=XCB_GC_FOREGROUND|XCB_GC_GRAPHICS_EXPOSURES;
    values[0]=col;
    values[1]=0;
    xcb_create_gc(device_context.connection, fill_colour_gc, pixmap, mask, values);
    cached_solid_fill_colour_graphics_contexts[col]=fill_colour_gc;
  }else{
    fill_colour_gc=cached_solid_fill_colour_graphics_contexts[col];
  }
}


void RTTXCB::rectangle(int x,int y,int w,int h,COLORREF cr){
}


void RTTXCB::rectangle_fill(s16 x,s16 y,s16 w,s16 h,COLORREF cr,COLORREF cr_fill){
  // +y is down
  assert(w>=0);
  assert(h>=0);
  set_fill_colour(cr_fill);
  xcb_rectangle_t rect={x,y,(u16)w,(u16)h};
  xcb_poly_fill_rectangle(device_context.connection, pixmap, fill_colour_gc, 1, &rect);
  if (cr!=cr_fill){
    set_fill_colour(cr);
    xcb_poly_rectangle(device_context.connection, pixmap, fill_colour_gc, 1, &rect);
  }
}


void RTTXCB::window_lines(int title_bar_h,COLORREF border_col,COLORREF title_bar_col){
}


void RTTXCB::clear(){
  if (background_col){
    set_fill_colour(background_col);
  }else{
    set_fill_colour(black_c);
  }
  xcb_rectangle_t rect={0,0,(u16)pixmap_w,(u16)pixmap_h};
  xcb_poly_fill_rectangle(device_context.connection, pixmap, fill_colour_gc, 1, &rect);
}


RTTXCB::RTTXCB(RTT_DC &dc,int _w,int _h,COLORREF _background_col,const char* _name):w(_w),
                                                                                    h(_h),
                                                                                    background_col(_background_col),
                                                                                    bitmap(0),
                                                                                    bitmap_dc(0),
                                                                                    name(_name),
                                                                                    first_time(true){
  //  assert(main_OS_window_dc);
  last_col=0;
  font_plotting_initialised=false;
  pixmap=0;
  pixmap_w=0;
  pixmap_h=0; 
  device_context=dc;
  make_bitmap();
  check_initialisation_of_debug_gfx();
  printf("RTTXCB::RTTXCB(RTT_DC, w=%d, h=%d, col=%8x, name=\"%s\")\n",_w,_h,_background_col,_name); 
}


void RTTXCB::set_OS_device_context(RTT_DC &dc){
  device_context=dc;
}


void RTTXCB::paint(){
}


void RTTXCB::RTTXCB_copy_area_checked(){
}


void RTTXCB::blit_entire_bitmap_to_screen(int x,int y,int main_winow_h){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_entire_bitmap_to_screen name=%s\n",name);
  //    printf("RTTXCB::blit_entire_bitmap_to_screen(x=%d, y=%d ,main_winow_h=%d)\n",x,y,main_winow_h);
#endif
  pthread_mutex_lock(&xcb_access_gating_mutex);
  //  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, pixmap, device_context.window, graphics_context, 0, 0, x, main_winow_h-y, image->width, image->height);
  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, pixmap, device_context.window, graphics_context, 0, 0, x, main_winow_h-y, w, h);
  xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
  if (error){
    printf("  Could not xcb_copy_area_checked in blit_entire_bitmap_to_screen\n");
    free(error);
    return;
  }
#if DO_FLUSH_AND_CHECK==1
  check_xcb_conncetion_error();
  xcb_flush (device_context.connection);
  check_xcb_conncetion_error();
#endif
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::blit_bitmap_to_screen_debug_rect(int sx,int sy,int sw,int sh,int bitmap_x,int bitmap_y,int main_winow_h){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_to_screen_debug_rect\n");
  //    printf("RTTXCB::blit_bitmap_to_screen_debug_rect(sx=%d, sy=%d, sw=%d, sh=%d, bitmap_x=%d,bitmap_y=%d,main_winow_h=%d, %d ,%d)\n",sx,sy,sw,sh,bitmap_x,bitmap_y,main_winow_h);
#endif
  #if SEE_DEBUG_CONSTRUCTIONS==1
  s32 xcb_x=sx;
  s32 xcb_y=main_winow_h-sy-sh;
  s32 xcb_w=sw;
  s32 xcb_h=sh;
  xcb_rectangle_t rectangles[] = {
    { (s16)xcb_x, (s16)xcb_y, (u16)xcb_w, (u16)xcb_h },
  };
  pthread_mutex_lock(&blit_mutex);
  xcb_poly_rectangle (device_context.connection, device_context.window, graphics_context, 1, rectangles);
  pthread_mutex_unlock(&blit_mutex);
  #endif
}


void RTTXCB::check_initialisation_of_debug_gfx(){
  if (!debug_stuff_initialised){
    printf("  ID's generated RTTXCB::check_initialisation_of_debug_gfx\n");
    debug_stuff_initialised=true;
    u32 mask=XCB_GC_FUNCTION |
      //      XCB_GC_PLANE_MASK |
      XCB_GC_FOREGROUND |
      XCB_GC_BACKGROUND |
      XCB_GC_LINE_WIDTH |
      XCB_GC_LINE_STYLE |
      //      XCB_GC_CAP_STYLE |
      //      XCB_GC_JOIN_STYLE |
      XCB_GC_FILL_STYLE |
      //      XCB_GC_FILL_RULE |
      //      XCB_GC_TILE |
      //      XCB_GC_STIPPLE |
      //      XCB_GC_TILE_STIPPLE_ORIGIN_X |
      //      XCB_GC_TILE_STIPPLE_ORIGIN_Y |
      //      XCB_GC_FONT |
      //      XCB_GC_SUBWINDOW_MODE |
      XCB_GC_GRAPHICS_EXPOSURES
      //      XCB_GC_CLIP_ORIGIN_X |
      //      XCB_GC_CLIP_ORIGIN_Y |
      //      XCB_GC_CLIP_MASK |
      //      XCB_GC_DASH_OFFSET |
      //      XCB_GC_DASH_LIST |
      //      XCB_GC_ARC_MODE
      ;
    uint32_t values[] = {
      XCB_GX_COPY,
      goldenrod_cr,
      device_context.screen->black_pixel,
      1,
      XCB_LINE_STYLE_SOLID,
      XCB_FILL_STYLE_SOLID,
      0
    };
    debug_gc1=xcb_generate_id(device_context.connection);
    debug_gc2=graphics_context;
    //    u32 mask=XCB_GC_FUNCTION | XCB_GC_FOREGROUND | XCB_GC_BACKGROUND |  XCB_GC_LINE_WIDTH| XCB_GC_LINE_STYLE | XCB_GC_GRAPHICS_EXPOSURES;
    //    uint32_t values[] = {
    //      XCB_GX_SET,
    //      goldenrod_cr,
    //      device_context.screen->black_pixel,
    //      1,
    //      XCB_LINE_STYLE_ON_OFF_DASH,
    //      0
    //    };
    xcb_void_cookie_t cookie=xcb_create_gc_checked(device_context.connection, debug_gc1, device_context.window, mask, values );
    xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
    if (error){
      printf("  Could not xcb_copy_area_checked in blit_entire_bitmap_to_screen\n");
      free(error);
      return;
    }
    check_xcb_conncetion_error();
  }
}


void RTTXCB::blit_bitmap_to_screen(int sx,int sy,int sw,int sh,int bitmap_x,int bitmap_y,int main_winow_h){
  dc_static=device_context;
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_to_screen name=%s\n",name);
#endif
  assert(sw<=image->width);
  assert(sh<=image->height);
  assert(bitmap_x<=image->width);
  assert(bitmap_y<=image->height);
  assert(bitmap_x>=0);
  assert(bitmap_y>=0);
  //  bitmap_x=0;
  //  bitmap_y=0;
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, pixmap, device_context.window, graphics_context, bitmap_x, bitmap_y, sx, main_winow_h-sy-sh, sw, sh);
  xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
  if (error){
    printf("  Could not xcb_copy_area_checked in blit_bitmap_to_screen\n");
    free(error);
    return;
  }else{
    //    printf("xcb_copy_area OK\n");
  }
  #if SEE_DEBUG_CONSTRUCTIONS==1
    s32 xcb_x=sx;
    s32 xcb_y=main_winow_h-sy-sh;
    s32 xcb_w=sw;
    s32 xcb_h=sh;
    xcb_rectangle_t rectangles[] = {
      { (s16)xcb_x, (s16)xcb_y, (u16)xcb_w, (u16)xcb_h },
    };
  
    xcb_poly_rectangle (device_context.connection, device_context.window, graphics_context, 3, rectangles);
  #endif
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::draw_rectangle(s32 x,s32 y,s32 w,s32 h,COLORREF col){
  u32 mask=XCB_GC_FUNCTION | XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  u32 values[]={
    XCB_GX_COPY,
    col,
    dc_static.screen->black_pixel,
  };
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_change_gc(dc_static.connection, debug_gc1, mask, values );;
  xcb_rectangle_t rectangles[] = {
    { (s16)x, (s16)(main_window_h_static-y-h), (u16)w, (u16)h },
  };
  xcb_poly_fill_rectangle( dc_static.connection, dc_static.window, debug_gc1, 1, rectangles);
  xcb_poly_rectangle(      dc_static.connection, dc_static.window, debug_gc2, 1, rectangles);
  xcb_flush(               dc_static.connection);
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::draw_rectangle_y_at_top_of_screen(s32 x,s32 y,s32 w,s32 h,COLORREF col){
  u32 mask=XCB_GC_FUNCTION | XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  u32 values[]={
    XCB_GX_COPY,
    col,
    dc_static.screen->black_pixel,
  };
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_change_gc(dc_static.connection, debug_gc1, mask, values );;
  xcb_rectangle_t rectangles[] = {
    { (s16)x, (s16)(main_window_h_static-y-h), (u16)w, (u16)h },
  };
  xcb_poly_fill_rectangle( dc_static.connection, dc_static.window, debug_gc1, 1, rectangles);
  xcb_poly_rectangle(      dc_static.connection, dc_static.window, debug_gc2, 1, rectangles);
  xcb_flush(               dc_static.connection);
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}

bool RTTXCB_debug_mode=false;


void RTTXCB::blit_bitmap_to_screen_window_mode(int sx,int sy,int sw,int sh,s32 window_x,s32 window_y,int main_winow_h){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_to_screen_window_mode name=%s\n",name);
  //    printf("RTTXCB::blit_bitmap_to_screen_window_mode(sx=%d, sy=%d, sw=%d, sh=%d, window_x=%d,window_y=%d,main_winow_h=%d, %d ,%d)\n", sx, sy, sw, sh, window_x, window_y, main_winow_h);
#endif
  // sy is y-up screen space
  // window_y is y-up screen space
  dc_static=device_context;
  main_window_h_static=main_winow_h;
  // asumes its part of a window being exposed so determines the bitmap coords automatically
  //  printf("blit_bitmap_to_screen %d %d %d %d\n",sx,sy,sw,sh);
  assert(pixmap_w==image->width);
  assert(pixmap_h==image->height);
  //  assert(sw<=image->width);
  //  assert(sh<=image->height);
  s32 bitmap_x=sx-window_x;
  s32 rect_top_y=sy+sh;
  s32 bitmap_y=window_y-rect_top_y;
  assert(bitmap_x<=w);
  assert(bitmap_y<=h);
  assert(bitmap_x>=0);
  assert(bitmap_y>=0);
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, pixmap, device_context.window, graphics_context, bitmap_x, bitmap_y, sx, main_winow_h-sy-sh, sw, sh);
  xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
  if (error){
    printf("  Could not xcb_copy_area_checked in blit_bitmap_to_screen\n");
    free(error);
    return;
  }else{
    //    printf("xcb_copy_area OK\n");
  }
  //  draw_rectangle(sx,sy,sw,sh,rand_col());
  
  if (RTTXCB_debug_mode){
    s32 xcb_x=sx;
    s32 xcb_y=main_winow_h-sy-sh;
    s32 xcb_w=sw;
    s32 xcb_h=sh;
    xcb_rectangle_t rectangles[] = {
      { (s16)xcb_x, (s16)xcb_y, (u16)xcb_w, (u16)xcb_h },
    };
  
    xcb_poly_rectangle (device_context.connection, device_context.window, graphics_context, 1, rectangles);
  }
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::blit_bitmap_to_screen_correct_for_Y_axis_irregularity(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y,int main_winow_h,int sub_window_h){
}


void RTTXCB::blit_bitmap_rectangle_to_screen_X_window_coords(int screen_x,int screen_y,int screen_w,int screen_h,int bitmap_offset_x,int bitmap_offset_y){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_rectangle_to_screen_X_window_coords\n");
  //    printf("blit_bitmap_rectangle_to_screen_X_window_coords(screen_x=%d, screen_y=%d, screen_w=%d, screen_h=%d, bitmap_offset_x=%d, bitmap_offset_y=%d)\n", screen_x, screen_y, screen_w, screen_h, bitmap_offset_x,bitmap_offset_y);
#endif
  dc_static=device_context;
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, pixmap, device_context.window, graphics_context, bitmap_offset_x, bitmap_offset_y, screen_x, screen_y, screen_w, screen_h);
  xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
  if (error){
    printf("  Could not xcb_copy_area_checked in blit_bitmap_to_screen\n");
    free(error);
    return;
  }else{
    //    printf("xcb_copy_area OK\n");
  }
#if DO_FLUSH_AND_CHECK==1
  check_xcb_conncetion_error();
  xcb_flush (device_context.connection);
  check_xcb_conncetion_error();
#endif
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::blit_bitmap_to_self(RTTXCB *rttgdi_source,
                                 int dest_x,
                                 int dest_y,
                                 int width,
                                 int height,
                                 int source_x,
                                 int source_y,
                                 bool y_is_up){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_to_self(self=%s  source=%s)\n",name,rttgdi_source->name);
  //    printf("blit_bitmap_to_self(self=%s  rttgdi_source=%s,  dest_x=%d, dest_y=%d, width=%d, height=%d, source_x=%d, source_y=%d, y_is_up=%d)\n",name,rttgdi_source->name,dest_x,dest_y,width,height,source_x,source_y,y_is_up);
#endif
  //  if (break_now){
  //    if (strncmp(name,"combo",5)==0){
  //      assert(0);
  //    }
  //  }
  dc_static=device_context;
  s32 dest_y_up_or_not=dest_y;
  if (y_is_up){
    dest_y_up_or_not=h-dest_y;
  }
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, rttgdi_source->pixmap, pixmap, graphics_context, source_x, source_y, dest_x, dest_y_up_or_not, width, height);
  xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
  if (error){
    printf("  Could not xcb_copy_area_checked in blit_bitmap_to_self\n");
    free(error);
    return;
  }
#if DO_FLUSH_AND_CHECK==1
  xcb_flush (device_context.connection);
  check_xcb_conncetion_error();
#endif
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::blit_bitmap_to_self(RTTXCB &rttgdi_source,
                                 int dest_x,
                                 int dest_y,
                                 int width,
                                 int height,
                                 int source_x,
                                 int source_y,
                                 bool y_is_up){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_to_self(self=%s  source=%s)\n",name,rttgdi_source->name);
  //    printf("blit_bitmap_to_self(self=%s  rttgdi_source=%s,  dest_x=%d, dest_y=%d, width=%d, height=%d, source_x=%d, source_y=%d, y_is_up=%d)\n",name,rttgdi_source->name,dest_x,dest_y,width,height,source_x,source_y,y_is_up);
#endif
  //  if (break_now){
  //    if (strncmp(name,"combo",5)==0){
  //      assert(0);
  //    }
  //  }
  dc_static=device_context;
  s32 dest_y_up_or_not=dest_y;
  if (y_is_up){
    dest_y_up_or_not=h-dest_y;
  }
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, rttgdi_source.pixmap, pixmap, graphics_context, source_x, source_y, dest_x, dest_y_up_or_not, width, height);
  xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
  if (error){
    printf("  Could not xcb_copy_area_checked in blit_bitmap_to_self\n");
    free(error);
    return;
  }
#if DO_FLUSH_AND_CHECK==1
  xcb_flush (device_context.connection);
  check_xcb_conncetion_error();
#endif
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::blit_bitmap_to_self_stretched_to_fit(RTTXCB *rttgdi_prestretched_source,
                                                  int screen_x,
                                                  int screen_y,
                                                  int screen_w,
                                                  int screen_h,
                                                  int copy_from_rectangle_x,
                                                  int copy_from_rectangle_y,
                                                  int copy_from_w,
                                                  int copy_from_h){
#if DEBUG_PRINT_BLITS==1
  if (can_print_debug_spam)
    printf("  blit_bitmap_to_self_stretched_to_fit\n");
  //    printf("RTTXCB::blit_bitmap_to_self_stretched_to_fit(rttgdi_source=%s, dest_x=%d, dest_y=%d, width=%d, height=%d, source_x=%d, source_y=%d, y_is_up=%d)\n",rttgdi_prestretched_source->name, screen_x, screen_y, screen_w, screen_h, copy_from_rectangle_x, copy_from_rectangle_y, copy_from_w, copy_from_h);
#endif
  //  if (break_now){
  //    if (strcmp(name,"combo 0")==0){
  //      assert(0);
  //    }
  //  }
  dc_static=device_context;
  bool re_stretch_bitmap=false;
  if (!rttgdi_prestretched_source){
    re_stretch_bitmap=true;
  }else{
    if (rttgdi_prestretched_source->w!=screen_w || rttgdi_prestretched_source->h!=screen_h){
      re_stretch_bitmap=true;
    }
  }
  if (re_stretch_bitmap){
    rttgdi_prestretched_source->rescale_bitmap(screen_w,screen_h,copy_from_rectangle_x,copy_from_rectangle_y,copy_from_w,copy_from_h);
  }
  blit_bitmap_to_self(rttgdi_prestretched_source,screen_x,screen_y,screen_w,screen_h,0,0); // the prestretched bitmap is already exactly the right size to blit 1:1
}


void RTTXCB::line(s16 x1,s16 y1,s16 x2,s16 y2,COLORREF cr){
  // plot single line segment from x1,y1 to x2,y2
  if (cr!=last_col){
    select_col(cr);
  }
  xcb_point_t pts[2];
  pts[0]={x1,y1};
  pts[1]={x2,y2};
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_poly_line(device_context.connection, XCB_COORD_MODE_ORIGIN, pixmap, foreground, 2, pts);
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::circle(s16 x,s16 y,u16 r,COLORREF cr){
  select_col(cr);
  x-=r;
  y-=r;
  u16 d=r*2;
  xcb_arc_t pts[1]={x, y, d, d, 0,360<<6};
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_poly_arc(device_context.connection, pixmap, foreground, 1, pts);
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::ellipse(s16 x,s16 y,u16 w,u16 h,COLORREF cr,s16 start_ang,s16 arc_ang){
  select_col(cr);
  xcb_arc_t pts[1]={x, y, w, h, start_ang, arc_ang};
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_poly_arc(device_context.connection, pixmap, foreground, 1, pts);
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::start_render_to_backbuffer(){
  clear();
  //  saved_main_OS_window_dc=main_OS_window_dc;
  // main_OS_window_dc=this->bitmap_dc;
}


void RTTXCB::swap_back_buffer_to_main(){
  //  BitBlt(saved_main_OS_window_dc,0,0,w,h,main_OS_window_dc,0,0,SRCCOPY);
  //  main_OS_window_dc=saved_main_OS_window_dc;
}


RTTXCB::RTTXCB(RTT_DC &dc,const char *filename):pixels(0),
                                                name(filename),
                                                first_time(true){
  font_plotting_initialised=false;
  pixmap=0;
  device_context=dc;
  load_24bit_BMP(filename);
  check_initialisation_of_debug_gfx();
  printf("RTTXCB::RTTXCB(RTT_DC, \"%s\")\n",filename); 
}

map<COLORREF,xcb_gcontext_t> RTTXCB::cached_graphics_contexts;

void RTTXCB::select_col(COLORREF col){
  if (col!=last_col||first_time){
    first_time=false;
    if (cached_graphics_contexts.find(col)==cached_graphics_contexts.end()){
      printf("  setting up new cached colour %8x\n",col);
      //  if (!font_plotting_initialised){
      uint32_t values[2];
      uint32_t mask=0;
      font_plotting_initialised=true;
      pthread_mutex_lock(&xcb_access_gating_mutex);
      foreground=xcb_generate_id(device_context.connection);
      //    mask=XCB_GC_FOREGROUND|XCB_GC_GRAPHICS_EXPOSURES;
      mask=XCB_GC_FOREGROUND;
      //    values[0] = device_context.screen->black_pixel;
      values[0]=col;
      values[1]=0;
      xcb_create_gc(device_context.connection, foreground, pixmap, mask, values);
      pthread_mutex_unlock(&xcb_access_gating_mutex);
      cached_graphics_contexts[col]=foreground;
    }else{
      foreground=cached_graphics_contexts[col];
    }
    last_col=col;
  }
}


void RTTXCB::setup_font_plotting(COLORREF col){
  select_col(col);
  cur_font_point=0;
}


void RTTXCB::add_font_pixel(s16 xp,s16 yp){
  font_points[cur_font_point++]={xp,yp};
  if (cur_font_point>=MAX_FONT_POINTS_TO_CACHE){
    render_font_pixels();
  }
}


void RTTXCB::render_font_pixels(){
  if (cur_font_point){
    xcb_poly_point(device_context.connection, XCB_COORD_MODE_ORIGIN, pixmap, foreground, cur_font_point, font_points);
  }
  //  SetPixelV(bitmap_dc,xp,yp,col);
  cur_font_point=0;  
}


void RTTXCB::setpixel(s16 xp,s16 yp,COLORREF col){
  select_col(col);
  xcb_point_t p={xp,yp};
  xcb_poly_point(device_context.connection, XCB_COORD_MODE_ORIGIN, pixmap, foreground, 1, &p);
}


BOOL RTTXCB::load_24bit_BMP(const char*filename){
  bool OK=bmp.read_from_file(filename);
  if (!OK){
    printf("  ---------- bitmap %s not found ------------\n",filename);
    assert(0);
  }
  w=bmp.get_width();
  h=bmp.get_height();
  
  pixels=new u8[w*h*4];
  printf("  loading bitmap %s\n",filename);
  // fuck
  u8* p=pixels;
  for(u32 y=0;y<h;y++){
    for(u32 x=0;x<w;x++){
      RGBApixel px=bmp.get_pixel(x,y);
      *p++=px.Blue;
      *p++=px.Green;
      *p++=px.Red;
      *p++=255;
    }
  }
  /* create backing pixmap */
  pthread_mutex_lock(&xcb_access_gating_mutex);
  pixmap = xcb_generate_id(device_context.connection);
  xcb_create_pixmap(device_context.connection, 24, pixmap, device_context.window, w, h);
  /* create pixmap plot gc */
  uint32_t mask=XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  uint32_t values[2];
  values[0]=device_context.screen->black_pixel;
  values[1]=0xffffff;
  graphics_context = xcb_generate_id(device_context.connection);
  xcb_create_gc(device_context.connection, graphics_context, pixmap, mask, values);
  /* create image */
  image=create_true_colour_image_from_rgb_data(device_context.connection, w, h, pixels);
  if (image==NULL){
    assert(0);
    printf("  Cannot create iamge\n");
    //    xcb_disconnect(connection);
    //    return 1;
  }
  /* put the image into the pixmap */
  xcb_image_put(device_context.connection, pixmap, graphics_context, image, 0, 0, 0);
  //  xcb_image_destroy(image);
  xcb_flush(device_context.connection);
  check_xcb_conncetion_error();
  pthread_mutex_unlock(&xcb_access_gating_mutex);
  return true;
}


void RTTXCB::make_bitmap(bool i_am_a_window_resizing){
  /* create backing pixmap */
  if (i_am_a_window_resizing){
    if (pixmap){
      if (w>pixmap_w || h>pixmap_h){
        pixmap_w=(w+PIXMAP_RESIZING_GRANULARITY)&(0xFFFFFFFF^(PIXMAP_RESIZING_GRANULARITY-1));
        pixmap_h=(h+PIXMAP_RESIZING_GRANULARITY)&(0xFFFFFFFF^(PIXMAP_RESIZING_GRANULARITY-1));
      }else{
        return;
      }
    }else{
      pixmap_w=w;
      pixmap_h=h;
    }
  }else{
    pixmap_w=w;
    pixmap_h=h;
  }
  printf("  generating a new pixmap & image (%dx%d) for %s\n",w,h,name.size()?name.data():"*** empty string ***");
  pixels=0;
  pthread_mutex_lock(&xcb_access_gating_mutex);
  pixmap=xcb_generate_id(device_context.connection);
  xcb_create_pixmap(device_context.connection, 24, pixmap, device_context.window, pixmap_w, pixmap_h);
  //  xcb_flush(device_context.connection);
  //  check_xcb_conncetion_error();
  /* create pixmap plot gc */
  uint32_t mask = XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  uint32_t values[2];
  values[0]=device_context.screen->black_pixel;
  values[1]=0xffffff;
  graphics_context=xcb_generate_id(device_context.connection);
  xcb_create_gc(device_context.connection, graphics_context, pixmap, mask, values);
  //  xcb_flush(device_context.connection);
  //  check_xcb_conncetion_error();
  /* create image */
  image=create_true_colour_image(device_context.connection, pixmap_w, pixmap_h);
  if (image==NULL) {
    assert(0);
    printf("  Cannot create iamge\n");
    //    xcb_disconnect(connection);
    //    return 1;
  }
  //  xcb_flush(device_context.connection);
  //  check_xcb_conncetion_error();
  /* put the image into the pixmap */
  xcb_image_put(device_context.connection, pixmap, graphics_context, image, 0, 0, 0);
  xcb_flush(device_context.connection);
  check_xcb_conncetion_error();
  pthread_mutex_unlock(&xcb_access_gating_mutex);
  clear();
}


RTTXCB::RTTXCB(RTTXCB &rttxcb,u32 action):w(rttxcb.w),
                                          h(rttxcb.h),
                                          background_col(rttxcb.background_col),
                                          bitmap(0),
                                          bitmap_dc(0),
                                          name(rttxcb.name),
                                          first_time(true){
  // copy the image from the supplied RTTXCB
  // get pointer to the source pixels, they will never be deleted so always valid
  font_plotting_initialised=false;
  pixels=rttxcb.pixels;
  bmp=rttxcb.bmp;
  printf("RTTXCB::RTTXCB(RTTXCB &rttxcb,u32 action):\n"); 
  printf("  generating a new RTTXCB (%dx%d) from %s\n",w,h,name.size()?name.data():"*** empty string ***");
  //  assert(main_OS_window_dc);
  device_context=rttxcb.device_context;
  /* create backing pixmap */
  pthread_mutex_lock(&xcb_access_gating_mutex);
  pixmap = xcb_generate_id(device_context.connection);
  xcb_create_pixmap(device_context.connection, 24, pixmap, device_context.window, w, h);
  /* create pixmap plot gc */
  uint32_t mask=XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  uint32_t values[2];
  values[0]=device_context.screen->black_pixel;
  values[1]=0xffffff;
  graphics_context = xcb_generate_id (device_context.connection);
  xcb_create_gc (device_context.connection, graphics_context, pixmap, mask, values);
  switch(action){
    case RTTXCB_COPY_IMAGE_FROM_SUPPLIED_RTTXCB:{
      /* create image */
      image=create_empty_true_colour_image(device_context.connection, w, h);
      if (image==NULL) {
        assert(0);
        printf("  Cannot create iamge\n");
      }
      xcb_image_convert(rttxcb.image,image);
      /* put the image into the pixmap */
      xcb_image_put(device_context.connection, pixmap, graphics_context, image, 0, 0, 0);
      xcb_flush(device_context.connection);
      check_xcb_conncetion_error();
      break;
    }
    case RTTXCB_COPY_VERBATIM:{
      xcb_void_cookie_t cookie=xcb_copy_area_checked(device_context.connection, rttxcb.pixmap, pixmap, graphics_context, 0,0,0,0,w,h);
      xcb_generic_error_t *error=xcb_request_check(device_context.connection, cookie);
      if (error){
        printf("Could not xcb_copy_area_checked in blit_bitmap_to_self\n");
        free(error);
        return;
      }
      xcb_flush(device_context.connection);
      check_xcb_conncetion_error();
      break;
    }
  }
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


void RTTXCB::rescale_bitmap(int dest_w,int dest_h,int source_x,int source_y,int source_w,int source_h){
  if (pixels==0){
    printf("  not rescaling bitmap cos not got original source for it\n");
    return;
  }
  w=dest_w;
  h=dest_h;
  printf("  rescaling from %d,%d to %d,%d\n",bmp.get_width(),bmp.get_height(),w,h);
  delete pixels;
  pixels=new u8[w*h*4];

  u8* p=pixels;
  float xs;
  float ys;
  int bmw=bmp.get_width();
  int bmh=bmp.get_height();
  if (w){
    xs=(float)source_w/(float)w;
  }else{
    xs=0;
  }
  if (h){
    ys=(float)source_h/(float)h;
  }else{
    ys=0;
  }
  float yf=(float)source_y;
  for(u32 y=0;y<h;y++){
    float xf=(float)source_x;
    for(u32 x=0;x<w;x++){
      RGBApixel px=bmp.get_pixel((int)xf,(int)yf);
      *p++=px.Blue;
      *p++=px.Green;
      *p++=px.Red;
      *p++=255;
      xf+=xs;
    }
    yf+=ys;
  }
  /* create backing pixmap */
  printf("  generating a new pixmap & image (%dx%d) for %s inside RTTXCB::rescale_bitmap\n",w,h,name.size()?name.data():"*** empty string ***");
  pthread_mutex_lock(&xcb_access_gating_mutex);
  pixmap = xcb_generate_id(device_context.connection);
  xcb_create_pixmap(device_context.connection, 24, pixmap, device_context.window, w, h);
  /* create pixmap plot gc */
  uint32_t mask=XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  uint32_t values[2];
  values[0]=device_context.screen->black_pixel;
  values[1]=0xffffff;
  graphics_context=xcb_generate_id(device_context.connection);
  xcb_create_gc(device_context.connection, graphics_context, pixmap, mask, values);
  /* create image */
  image = create_true_colour_image_from_rgb_data(device_context.connection, w, h, pixels);
  if (image==NULL) {
    assert(0);
    printf("  Cannot create iamge\n");
  }
  /* put the image into the pixmap */
  xcb_image_put(device_context.connection, pixmap, graphics_context, image, 0, 0, 0);
  //  xcb_image_destroy(image)
  xcb_flush(device_context.connection);
  check_xcb_conncetion_error();
  //  xcb_image_destroy(oimage);
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}


RTTXCB::RTTXCB(RTTXCB &source,int dest_w,int dest_h,int source_x,int source_y,int source_w,int source_h,const char* _name):name(_name),
                                                                                                                           first_time(true){
  // copy the image resized from supplied RTTXCB
  // get pointer to the source pixels, they will never be deleted so always valid
  font_plotting_initialised=false;
  bmp=source.bmp;
  w=dest_w;
  h=dest_h;
  printf("RTTXCB::RTTXCB(RTTXCB , dest_w=%d, dest_h=%d, source_x=%d, source_y=%d, source_w=%d ,source_h=%d, name=\"%s\")\n",dest_w,dest_h,source_x,source_y,source_w,source_h,_name); 
  printf("  creating new RTTXCB rescaled from %d,%d to %d,%d\n",bmp.get_width(),bmp.get_height(),w,h);
  if (bmp.get_width()==0||bmp.get_height()==0){
    volatile int rt=1;
  }
  pixels=new u8[w*h*4];
  u8* p=pixels;
  float xs;
  float ys;
  int bmw=bmp.get_width();
  int bmh=bmp.get_height();
  if (w){
    xs=(float)source_w/(float)w;
  }else{
    xs=0;
  }
  if (h){
    ys=(float)source_h/(float)h;
  }else{
    ys=0;
  }
  float yf=(float)source_y;
  for(u32 y=0;y<h;y++){
    float xf=(float)source_x;
    for(u32 x=0;x<w;x++){
      RGBApixel px=bmp.get_pixel((int)xf,(int)yf);
      *p++=px.Blue;
      *p++=px.Green;
      *p++=px.Red;
      *p++=255;
      xf+=xs;
    }
    yf+=ys;
  }
  /* create backing pixmap */
  pthread_mutex_lock(&xcb_access_gating_mutex);
  pixmap = xcb_generate_id(device_context.connection);
  xcb_create_pixmap(device_context.connection, 24, pixmap, device_context.window, w, h);
  /* create pixmap plot gc */
  uint32_t mask=XCB_GC_FOREGROUND | XCB_GC_BACKGROUND;
  uint32_t values[2];
  values[0]=device_context.screen->black_pixel;
  values[1]=0xffffff;
  graphics_context = xcb_generate_id (device_context.connection);
  xcb_create_gc(device_context.connection, graphics_context, pixmap, mask, values);
  /* create image */
  image = create_true_colour_image_from_rgb_data(device_context.connection, w, h, pixels);
  if (image==NULL) {
    assert(0);
    printf("  Cannot create iamge\n");
  }
  /* put the image into the pixmap */
  xcb_image_put(device_context.connection, pixmap, graphics_context, image, 0, 0, 0);
  //  xcb_image_destroy(image)
  xcb_flush(device_context.connection);
  check_xcb_conncetion_error();
  pthread_mutex_unlock(&xcb_access_gating_mutex);
  check_initialisation_of_debug_gfx();
}


void RTTXCB::resample(int dest_w,int dest_h,int source_x,int source_y,int source_w,int source_h){
  // resample the image but dont resize the X pixmap
  // so can resample smaller or back to original size only
  //  return;
  w=dest_w;
  h=dest_h;
  if (bmp.get_width()==0||bmp.get_height()==0){
    volatile int rt=1;
  }
  u8* p=pixels;
  float xs;
  float ys;
  int bmw=bmp.get_width();
  int bmh=bmp.get_height();
  if (dest_w>image->width || dest_h>image->height){
    printf("  RTTXCB::resample RESAMPLING to original size, specified size was too large\n");
    w=bmw;
    h=bmh;
  }
  if (source_w==RESIZE_FROM_ORIG_BMP){
    source_w=bmw;
  }
  if (source_h==RESIZE_FROM_ORIG_BMP){
    source_h=bmh;
  }
  printf("  rescaling RTTXCB pixmap image from %d,%d to %d,%d\n",bmp.get_width(),bmp.get_height(),w,h);
  if (w){
    xs=(float)source_w/(float)w;
  }else{
    xs=0;
  }
  if (h){
    ys=(float)source_h/(float)h;
  }else{
    ys=0;
  }
  float yf=(float)source_y;
  for(u32 y=0;y<h;y++){
    float xf=(float)source_x;
    for(u32 x=0;x<w;x++){
      RGBApixel px=bmp.get_pixel((int)xf,(int)yf);
      xcb_image_put_pixel(image,x,y,*((u32*)&px));
      xf+=xs;
    }
    yf+=ys;
  }
  /* put the image into the pixmap */
  pthread_mutex_lock(&xcb_access_gating_mutex);
  xcb_image_put(device_context.connection, pixmap, graphics_context, image, 0, 0, 0);
  xcb_flush(device_context.connection);
  check_xcb_conncetion_error();
  pthread_mutex_unlock(&xcb_access_gating_mutex);
}
 
