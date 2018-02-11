#include "RTT_2.h"
#include "doit_all.h"
#include <vector>
#include "RTT_patest_wire.h"
#include "RTTGDIXCB.h"
#include "OS_interface.h"
#include "main.h"
#include "bitmaps/cde_backdrops/Ankh.bm"
#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <xcb/xcb_keysyms.h>
#include <assert.h>
#include "RTT_keysyms.h"

using namespace std;

#include "main.h"

xcb_connection_t *connection;
xcb_drawable_t    window;
xcb_gcontext_t    foreground;
xcb_screen_t     *screen;

char text_message[64]="* running.... asc ";


vector<string> info_text;
int info_text_line_height=13;

bool mls_playback=false; 

static xcb_gc_t gc_font_get(xcb_connection_t *c,xcb_screen_t *screen,xcb_window_t window,const char *font_name);
static void     button_draw(xcb_connection_t *c,xcb_screen_t *screen,xcb_window_t window,int16_t x1,int16_t y1,const char*label);
static void       text_draw(xcb_connection_t *c,xcb_screen_t *screen,xcb_window_t window,int16_t x1,int16_t y1,const char*label);
static void      cursor_set(xcb_connection_t *c,xcb_screen_t *screen,xcb_window_t window,int cursor_id);


static xcb_keycode_t firstKeycode = 255;
static xcb_keycode_t lastKeycode  = 0;

// We use a simple array instead of a map => constant time lookup
// xcb_keycode_t can only contain 256 distinct values
static xcb_keysym_t keysymMap[256];

static xcb_keysym_t* keysyms;  // the whole keysym table from the x server
static uint8_t keysymsPerKeycode;
xcb_key_symbols_t *syms;



button_data buttons[NUM_BUTTONS]={
  button_data("Generate MLS sequence",130,24,100,20),
  button_data("Initialize audio",300,24,100,20),
  button_data("Shutdown audio",300,44,100,20),
  button_data("Toggle MLS playback",300,64,100,20),
  button_data("Determine system audio latency",300,84,100,20),
  button_data("Start single MLS sample run",300,104,100,20),
  button_data("Start multiple MLS sample runs",300,124,100,20),
};


xcb_point_t spinner_pos={250,10};
xcb_point_t info_text_pos={10,300};

int spinner_font_pixel_size=10;
#define RTT_EXPOSE_NO_ACTION 0
#define RTT_EXPOSE_INDEX_SPINNER 255

void invalidate(int x,int y,int w,int h){
  xcb_expose_event_t invalidate_event;
  invalidate_event.window=window;
  invalidate_event.response_type=XCB_EXPOSE;
  invalidate_event.x=x;
  invalidate_event.y=y;
  invalidate_event.width=x+w;
  invalidate_event.height=y+h;
  invalidate_event.count=RTT_EXPOSE_INDEX_SPINNER;
  xcb_send_event(connection,false,window,XCB_EVENT_MASK_EXPOSURE,(char*)&invalidate_event);
  xcb_flush(connection);
}

u32 mouse_x=0;
u32 mouse_y=0;

int spinner_char=0;
char s_chars[]="|/-\\";

void handler(sigval val){
  text_message[0]=s_chars[spinner_char];
  spinner_char++;
  if (wm.key_alt_pressed){
    text_message[14]='A';
  }else{
    text_message[14]='a';
  }
  if (wm.key_shift_pressed){
    text_message[15]='S';
  }else{
    text_message[15]='s';
  }
  if (wm.key_control_pressed){
    text_message[16]='C';
  }else{
    text_message[16]='c';
  }
  if(spinner_char>=strlen(s_chars)-1){
    spinner_char=0;
  }
  sprintf(text_message+18,"mouse %4d %4d",mouse_x,main_window_h-mouse_y);
  invalidate(spinner_pos.x,spinner_pos.y,spinner_font_pixel_size,spinner_font_pixel_size);
  wm.update();
}

bool can_send_mouse_coords=true;

void mouse_move_timer_handler(sigval val){
  can_send_mouse_coords=true;
}



timer_t tid;
timer_t mouse_move_tid;

void set_timer(){
  sigevent sig;
  sig.sigev_notify=SIGEV_THREAD;
  sig.sigev_notify_function=handler;
  sig.sigev_notify_attributes=NULL;
  sig.sigev_value.sival_ptr=&tid;
  timer_create(CLOCK_REALTIME,&sig,&tid);

  itimerspec t={{0,200000000},{1,0}};
  timer_settime(tid,0,&t,0);

  //  sigevent sig;
  sig.sigev_notify=SIGEV_THREAD;
  sig.sigev_notify_function=mouse_move_timer_handler;
  sig.sigev_notify_attributes=NULL;
  sig.sigev_value.sival_ptr=&mouse_move_tid;
  timer_create(CLOCK_REALTIME,&sig,&mouse_move_tid);

  itimerspec t2={{0,1000000000/MOUSE_UPDATE_RATE},{1,0}};
  timer_settime(mouse_move_tid,0,&t2,0);
}


static void button_draw(xcb_connection_t *c,xcb_screen_t *screen,xcb_window_t window,int16_t x1,int16_t y1,const char *label){
  xcb_point_t          points[5];
  xcb_void_cookie_t    cookie_gc;
  xcb_void_cookie_t    cookie_line;
  xcb_void_cookie_t    cookie_text;
  xcb_generic_error_t *error;
  xcb_gcontext_t       gc;
  int16_t              width;
  int16_t              height;
  uint8_t              length;
  int16_t              inset;

  length=strlen(label);
  inset=2;
  gc=gc_font_get(connection,screen,window,"7x13");

  width=7*length+2*(inset+1);
  height=13+2*(inset+1);
  points[0].x=x1;
  points[0].y=y1;
  points[1].x=x1+width;
  points[1].y=y1;
  points[2].x=x1+width;
  points[2].y=y1-height;
  points[3].x=x1;
  points[3].y=y1-height;
  points[4].x=x1;
  points[4].y=y1;
  cookie_line=xcb_poly_line_checked(c,XCB_COORD_MODE_ORIGIN,window,gc,5,points);

  error=xcb_request_check(connection,cookie_line);
  if(error) {
    fprintf(stderr,"ERROR: can't draw lines : %d\n",error->error_code);
    xcb_disconnect(c);
    exit(-1);
  }

  cookie_text=xcb_image_text_8_checked(connection,length,window,gc,x1+inset+1,y1-inset-1,label);
  error=xcb_request_check(connection,cookie_text);
  if(error) {
    fprintf(stderr,"ERROR: can't paste text : %d\n",error->error_code);
    xcb_disconnect(c);
    exit(-1);
  }

  cookie_gc=xcb_free_gc(connection,gc);
  error=xcb_request_check(connection,cookie_gc);
  if(error) {
    fprintf(stderr,"ERROR: can't free gc : %d\n",error->error_code);
    xcb_disconnect(c);
    exit(-1);
  }
}


static void text_draw(xcb_connection_t *connection,xcb_screen_t *screen,xcb_window_t window,int16_t x1,int16_t y1,const char *label){
  xcb_void_cookie_t    cookie_gc;
  xcb_void_cookie_t    cookie_text;
  xcb_generic_error_t *error;
  xcb_gcontext_t       gc;
  uint8_t              length;

  length=strlen(label);
  gc=gc_font_get(connection,screen,window,"7x13");

  cookie_text=xcb_image_text_8_checked(connection,length,window,gc,x1,y1,label);
  error=xcb_request_check(connection,cookie_text);
  if(error) {
    fprintf(stderr,"ERROR: can't paste text : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }

  cookie_gc=xcb_free_gc(connection,gc);
  error=xcb_request_check(connection,cookie_gc);
  if(error) {
    fprintf(stderr,"ERROR: can't free gc : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }
}



static xcb_gc_t gc_font_get(xcb_connection_t *connection,xcb_screen_t *screen,xcb_window_t window,const char *font_name) {
  uint32_t             value_list[3];
  xcb_void_cookie_t    cookie_font;
  xcb_void_cookie_t    cookie_gc;
  xcb_generic_error_t *error;
  xcb_font_t           font;
  xcb_gcontext_t       gc;
  uint32_t             mask;

  font=xcb_generate_id(connection);
  cookie_font=xcb_open_font_checked(connection,font,strlen(font_name),font_name);

  error=xcb_request_check(connection,cookie_font);
  if(error) {
    fprintf(stderr,"ERROR: can't open font : %d\n",error->error_code);
    xcb_disconnect(connection);
    return (xcb_gc_t)-1;
  }

  gc=xcb_generate_id(connection);
  mask=XCB_GC_FOREGROUND|XCB_GC_BACKGROUND|XCB_GC_FONT;
  value_list[0]=screen->black_pixel;
  value_list[1]=screen->white_pixel;
  value_list[2]=font;
  cookie_gc=xcb_create_gc_checked(connection,gc,window,mask,value_list);
  error=xcb_request_check(connection,cookie_gc);
  if(error) {
    fprintf(stderr,"ERROR: can't create gc : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }

  cookie_font=xcb_close_font_checked(connection,font);
  error=xcb_request_check(connection,cookie_font);
  if(error) {
    fprintf(stderr,"ERROR: can't close font : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }
  return (xcb_gc_t)gc;
}



static void cursor_set(xcb_connection_t *connection,xcb_screen_t *screen,xcb_window_t window,int cursor_id) {
  uint32_t             values_list[3];
  xcb_void_cookie_t    cookie_font;
  xcb_void_cookie_t    cookie_gc;
  xcb_generic_error_t *error;
  xcb_font_t           font;
  xcb_cursor_t         cursor;
  xcb_gcontext_t       gc;
  uint32_t             mask;
  
  uint32_t             value_list;
  font=xcb_generate_id(connection);
  cookie_font=xcb_open_font_checked(connection,font,strlen("cursor"),"cursor");
  error=xcb_request_check(connection,cookie_font);
  if(error) {
    fprintf(stderr,"ERROR: can't open font : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }

  cursor=xcb_generate_id(connection);
  xcb_create_glyph_cursor(connection,cursor,font,font,cursor_id,cursor_id+1,0,0,0,0,0,0);

  gc=xcb_generate_id(connection);
  mask=XCB_GC_FOREGROUND|XCB_GC_BACKGROUND|XCB_GC_FONT;
  values_list[0]=screen->black_pixel;
  values_list[1]=screen->white_pixel;
  values_list[2]=font;
  cookie_gc=xcb_create_gc_checked(connection,gc,window,mask,values_list);
  error=xcb_request_check(connection,cookie_gc);
  if(error) {
    fprintf(stderr,"ERROR: can't create gc : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }

  mask=XCB_CW_CURSOR;
  value_list=cursor;
  xcb_change_window_attributes(connection,window,mask,&value_list);

  xcb_free_cursor(connection,cursor);

  cookie_font=xcb_close_font_checked(connection,font);
  error=xcb_request_check(connection,cookie_font);
  if(error) {
    fprintf(stderr,"ERROR: can't close font : %d\n",error->error_code);
    xcb_disconnect(connection);
    exit(-1);
  }
}



/* stard/end point storage for line drawing */
xcb_point_t line_start={0,0};
xcb_point_t line_end={200,200};



/* geometric objects */
xcb_point_t points[]={
  {10,10},
  {10,20},
  {20,10},
  {20,20}};

xcb_point_t polyline[]={
  {50,10},
  {5,20},     /* rest of points are relative */
  {25,-20},
  {10,10}};

xcb_segment_t segments[]={
  {100,10,140,30},
  {110,25,130,60}};

xcb_rectangle_t rectangles[]={
  {10,50,40,20},
  {80,50,10,40}};

xcb_arc_t arcs[]={
  {10,100,60,40,0,90<<6},
  {90,100,55,40,0,270<<6}};


void check_xcb_conncetion_error(){
  int err=xcb_connection_has_error(connection);
  switch(err){
    case XCB_CONN_ERROR:
      printf("connection error: XCB_CONN_ERROR\n");
      assert(0);
      break;
    case XCB_CONN_CLOSED_EXT_NOTSUPPORTED:
      printf("connection error: XCB_CONN_CLOSED_EXT_NOTSUPPORTED\n");
      break;
    case XCB_CONN_CLOSED_MEM_INSUFFICIENT:
      printf("connection error: XCB_CONN_CLOSED_MEM_INSUFFICIENT\n");
      break;
    case XCB_CONN_CLOSED_REQ_LEN_EXCEED:
      printf("connection error: XCB_CONN_CLOSED_REQ_LEN_EXCEED\n");
      break;
    case XCB_CONN_CLOSED_PARSE_ERR:
      printf("connection error: XCB_CONN_CLOSED_PARSE_ERR\n");
      break;
    case XCB_CONN_CLOSED_INVALID_SCREEN:
      printf("connection error: XCB_CONN_CLOSED_INVALID_SCREEN\n");
      break;
  }
}


void start_MLSSA(){
  cout<<endl<<endl<<"calling doit"<<endl<<endl;
  if (!audio_is_initialized){
    init_LFSR_MLS();
  }
}


void handle_button_press(int button_presssed){
  switch(button_presssed){
    case INDEX_START_BUTTON:
      start_MLSSA();
      break;
    case INDEX_SINGLE_PLAY_BUTTON:
      cout<<endl<<endl<<"outputting MLS as audio"<<endl<<endl;
      initialize_audio_and_start_streaming();
      break;
    case INDEX_SHUTDOWN_AUDIO_BUTTON:
      cout<<endl<<endl<<"Shutting down audio"<<endl<<endl;
      shutdown_everything();
      break;
    case INDEX_STOP_MLS_PLAYBACK:
      cout<<endl<<endl<<"Toggling MLS playback"<<endl<<endl;
      mls_playback^=true;
      break;
    case INDEX_DETERMINE_SYSTEM_AUDIO_LATENCY:
      start_latency_measurement();
      break;
    case INDEX_SINGLE_MLS_SAMPLE_RUN:
      break;
    case INDEX_MULTIPLE_MLS_SAMPLE_RUN:
      break;
  }
}


int main() {
  xcb_generic_event_t *e;
  uint32_t             mask=0;
  uint32_t             values[2];
  uint8_t               is_hand=0;
  int                   screen_number;
  xcb_screen_iterator_t screen_iter;
  const xcb_setup_t    *setup;
  xcb_image_t *image;
  uint8_t *image32;
  //  xcb_size_hints_t *hints;
  
  info_text.push_back("info text:");
  info_text.push_back("max delta-vol silence=");
  /* Open the connection to the X server */
  connection=xcb_connect(NULL,&screen_number);
  if(!connection) {
    fprintf(stderr,"ERROR: can't connect to an X server\n");
    return -1;
  }
  //  c = xcb_connect (NULL, NULL);

  /* Get the first screen */
  setup=xcb_get_setup(connection);

  screen=NULL;
  screen_iter=xcb_setup_roots_iterator(setup);
  for(; screen_iter.rem!=0; --screen_number,xcb_screen_next(&screen_iter))
    if(screen_number==0) {
      screen=screen_iter.data;
      break;
    }
  if(!screen) {
    fprintf(stderr,"ERROR: can't get the current screen\n");
    xcb_disconnect(connection);
    return -1;
  }

  /* create image */
  image = create_true_colour_image(connection, 256, 256);
  if (image == NULL) {
    printf ("Cannot create iamge\n");
    //    xcb_disconnect(connection);
    //    return 1;
  }else{
    image32 = image->data;
  }
  //  screen = xcb_setup_roots_iterator (xcb_get_setup (c)).data;


  //  /* set size hits on window */
  //  hints = xcb_alloc_size_hints();
  //  xcb_size_hints_set_max_size(hints, image->width,image->height);
  //  xcb_size_hints_set_min_size(hints, image->width,image->height);
  //  xcb_set_wm_size_hints(c, w, WM_NORMAL_HINTS, hints);

  /* Create black (foreground) graphic context */
  window=screen->root;

  foreground=xcb_generate_id(connection);
  mask=XCB_GC_FOREGROUND|XCB_GC_GRAPHICS_EXPOSURES;
  values[0]=screen->black_pixel;
  values[1]=0;
  xcb_create_gc(connection,foreground,window,mask,values);

  /* Ask for our window's Id */
  window=xcb_generate_id(connection);

  /* Create the window */
  mask=XCB_CW_BACK_PIXEL|XCB_CW_EVENT_MASK;
  values[0]=screen->white_pixel;
  values[1]=XCB_EVENT_MASK_KEY_RELEASE|
    XCB_EVENT_MASK_KEY_PRESS|
    XCB_EVENT_MASK_BUTTON_RELEASE|
    XCB_EVENT_MASK_BUTTON_MOTION|
    XCB_EVENT_MASK_BUTTON_PRESS|
    XCB_EVENT_MASK_RESIZE_REDIRECT|
    XCB_EVENT_MASK_EXPOSURE|
    XCB_EVENT_MASK_POINTER_MOTION;
  
  xcb_create_window(connection,                   /* Connection    */
										XCB_COPY_FROM_PARENT,         /* depth         */
										window,                       /* window Id     */
										screen->root,                 /* parent window */
										main_window_x,main_window_y,  /* x, y          */
										main_window_w,main_window_h,  /* width, height */
										10,                           /* border_width  */
										XCB_WINDOW_CLASS_INPUT_OUTPUT,/* class         */
										screen->root_visual,          /* visual        */
										mask,values);                 /* masks         */
  /* Map the window on the screen */
  xcb_map_window(connection,window);
  /* We flush the request */
  xcb_flush(connection);

  syms = xcb_key_symbols_alloc(connection);

  RTT_DC dc;
  dc.connection=connection;
  dc.window=window;
  dc.screen=screen;
  // init the window manager
  wm.init(dc);
  set_timer();
  bool already_here=false;
  check_xcb_conncetion_error();
  wm.update_all_windows_contents();
  wm.render_all_windows_to_the_screen();
  while((e=xcb_wait_for_event(connection))) {
    switch(e->response_type & ~0x80) {
      case XCB_RESIZE_REQUEST:{
        xcb_resize_request_event_t *resize=(xcb_resize_request_event_t*)e;
        if (resize->width > 0)  main_window_w =resize->width;
        if (resize->height > 0) main_window_h=resize->height;
        break;
      }
      case XCB_EXPOSE: {
        xcb_expose_event_t *ev=(xcb_expose_event_t*)e;
        switch(ev->count){
          case RTT_EXPOSE_INDEX_SPINNER:
            draw_spinner();
            break;
          default:
            draw_button_stuff();
            draw_stuff();
            draw_info_text();
            wm.wm_redraw_exposed_screen_rectangle(ev->x ,main_window_h-ev->y-ev->height ,ev->width ,ev->height);
            //            wm.paint_all();
            break;
        }
        break;
      }
      case XCB_BUTTON_RELEASE: {
        xcb_button_press_event_t *ev;
        ev=(xcb_button_press_event_t *)e;
        s32 x=ev->event_x;
        s32 y=main_window_h-ev->event_y;
        wm.mouse_move(x,y);
        if (ev->detail==XCB_BUTTON_INDEX_1){
          wm.mouse_click(MOUSE_LEFT_UP,x,y);
        }
        if (ev->detail==XCB_BUTTON_INDEX_2){
          wm.mouse_click(MOUSE_MIDDLE_UP,x,y);
        }
        if (ev->detail==XCB_BUTTON_INDEX_3){
          wm.mouse_click(MOUSE_RIGHT_UP,x,y);
        }
        break;
      }
      case XCB_BUTTON_PRESS: {
        printf("XCB_BUTTON_PRESS\n");
        //        break_now=true;
        //        break;
        xcb_button_press_event_t *ev;
        ev=(xcb_button_press_event_t *)e;
        int length;
        s32 x=ev->event_x;
        s32 y=main_window_h-ev->event_y;
        wm.mouse_move(x,y);
        switch(ev->detail){
          case XCB_BUTTON_INDEX_1:
            wm.mouse_click(MOUSE_LEFT_DOWN,x,y);
            break;
          case XCB_BUTTON_INDEX_2:
            wm.mouse_click(MOUSE_MIDDLE_DOWN,x,y);
            break;
          case XCB_BUTTON_INDEX_3:
            wm.mouse_click(MOUSE_RIGHT_DOWN,x,y);
            break;
          case XCB_BUTTON_INDEX_4:
            wm.mouse_click(MOUSE_WHEEL_UP,x,y);
            break;
          case XCB_BUTTON_INDEX_5:
            wm.mouse_click(MOUSE_WHEEL_DOWN,x,y);
            break;
        }
        
        length=strlen("click here to change cursor");
        //        polyline[0].x=ev->event_x;
        //        polyline[0].y=ev->event_y;
        //        xcb_clear_area(connection,1,window,ev->event_x-20,ev->event_y-20,ev->event_x+20,ev->event_y+20);

        if((ev->event_x>=(wm.main_w-7*length)/2)&&(ev->event_x<=((wm.main_w-7*length)/2+7*length+6))&&(ev->event_y>=(wm.main_h-16)-19)&&(ev->event_y<=((wm.main_h-16)))) {
          
          is_hand=1-is_hand;
        }

        int button_presssed=-1;
        for(int i=0;i<NUM_BUTTONS;i++){
          if (buttons[i].check_button(ev->event_x,ev->event_y)){
            button_presssed=i;
            break;
          }
        }

        switch(button_presssed){
          case INDEX_START_BUTTON:
            cout<<endl<<endl<<"calling doit"<<endl<<endl;
            if (!audio_is_initialized){
              init_LFSR_MLS();
            }
            break;
          case INDEX_SINGLE_PLAY_BUTTON:
            cout<<endl<<endl<<"outputting MLS as audio"<<endl<<endl;
            initialize_audio_and_start_streaming();
            break;
          case INDEX_SHUTDOWN_AUDIO_BUTTON:
            cout<<endl<<endl<<"Shutting down audio"<<endl<<endl;
            shutdown_everything();
            break;
          case INDEX_STOP_MLS_PLAYBACK:
            cout<<endl<<endl<<"Toggling MLS playback"<<endl<<endl;
            mls_playback^=true;
            break;
          case INDEX_DETERMINE_SYSTEM_AUDIO_LATENCY:
            start_latency_measurement();
            break;
          case INDEX_SINGLE_MLS_SAMPLE_RUN:
            break;
          case INDEX_MULTIPLE_MLS_SAMPLE_RUN:
            break;
        }
	  
        is_hand?cursor_set(connection,screen,window,58):cursor_set(connection,screen,window,68);
        xcb_flush(connection);

        //        static int once=true;
        //        if (once){
        //          once=false;
        //          wm.fake_window_move();
        //        }
        break;
        //XExposeEvent redraw_rect;
      }
      case XCB_KEY_RELEASE: {
        xcb_key_release_event_t *ev;
        ev=(xcb_key_release_event_t *)e;
        xcb_keysym_t sym;
        xcb_keysym_t sym0;
        xcb_keysym_t sym1;
        sym0=xcb_key_press_lookup_keysym(syms, ev, 0);
        sym1=xcb_key_press_lookup_keysym(syms, ev, 1);
        if (xcb_is_keypad_key(sym0)) {
          sym=sym1;
        }else{
          if ((ev->state & XCB_MOD_MASK_SHIFT)){
            if (sym1==0){
              // if its shift by itself send shift keysym not 0
              sym=sym0;
            }else{              
              sym=sym1;
            }
          }else{
            sym=sym0;
          }
        }
        u8 ASCII;
        bool is_ascii=keysym_to_RTT(sym,ASCII);
        wm.key(KEYPRESS_UP,sym,is_ascii,ASCII);

        switch(ev->detail) {
          /* ESC */
          case 9:
            free(e);
            xcb_disconnect(connection);
            shutdown_everything(); // audio shutdown
            return 0;
        }
        break;
      }
      case XCB_KEY_PRESS: {
        xcb_key_press_event_t *ev;
        ev=(xcb_key_press_event_t *)e;
        xcb_keysym_t sym;
        xcb_keysym_t sym0;
        xcb_keysym_t sym1;
        sym0=xcb_key_press_lookup_keysym(syms, ev, 0);
        sym1=xcb_key_press_lookup_keysym(syms, ev, 1);
        if (xcb_is_keypad_key(sym0)) {
          sym=sym1;
        }else{
          if ((ev->state & XCB_MOD_MASK_SHIFT)){
            sym=sym1;
          }else{
            sym=sym0;
          }
        }
        u8 ASCII;
        bool is_ascii=keysym_to_RTT(sym,ASCII);

        wm.key(KEYPRESS_DOWN,sym,is_ascii,ASCII);
      
        switch(ev->detail) {
          /* ESC */
          case 9:
            free(e);
            xcb_disconnect(connection);
            shutdown_everything(); // audio shutdown
            return 0;
        }
        break;
      }
      case XCB_MOTION_NOTIFY: {
        xcb_motion_notify_event_t *ev=(xcb_motion_notify_event_t  *)e;
        if (already_here){
          printf("already in here\n");
        }
        already_here=true;
        static u32 last_x;
        static u32 last_y;
        mouse_x=ev->event_x;
        mouse_y=ev->event_y;
        //        can_send_mouse_coords=true;
        if (can_send_mouse_coords){
          if (last_x!=mouse_x || last_y!=mouse_y){
            wm.mouse_move(mouse_x,wm.main_h-mouse_y);
            can_send_mouse_coords=false;
          }
        }
        last_x=mouse_x;
        last_y=mouse_y;
        //      /*
        //	1. clear the area on the win between line_start and mouse_pos (or whole win)
        //	2. update mouse_pos
        //	3. draw line from line_start to mouse_pos
        //      */
        //xcb_clear_area(c, 1, win,ev->event_x-20, ev->event_y-20, ev->event_x+20,ev->event_y+20);
        //xcb_point_t pt1 = { ev->event_x-20,ev->event_y-20} ;
        //xcb_point_t pt2 = { ev->event_x+20,ev->event_y+20} ;
        //xcb_point_t points[] = {pt1,pt2};
        //xcb_poly_line (c, XCB_COORD_MODE_PREVIOUS, win, foreground, 2, points);
        //
        //xcb_flush(c);
        already_here=false;
        break;
      }
      case 0:
        printf("Received X11 error errm\n");
        break;
      default: {
        /* Unknown event type, ignore it */
        break;
      }
    }
    /* Free the Generic Event */
    free(e);
  }

  return 0;
}



void draw_stuff() {
  /* We draw the points */
  xcb_poly_point(connection,XCB_COORD_MODE_ORIGIN,window,foreground,4,points);
  
  /* We draw the polygonal line */
  xcb_poly_line(connection,XCB_COORD_MODE_PREVIOUS,window,foreground,4,polyline);

  /* We draw the segements */
  xcb_poly_segment(connection,window,foreground,2,segments);

  /* We draw the rectangles */
  xcb_poly_rectangle(connection,window,foreground,2,rectangles);

  /* We draw the arcs */
  xcb_poly_arc(connection,window,foreground,2,arcs);

  /* We flush the request */
  xcb_flush(connection);
}



void draw_button_wrapper(button_data &b){
  button_draw(connection,screen,window,b.x,b.y,b.txt);
}



void draw_button_stuff() {
  char *text;
  text=(char*)"click here to change cursor";

  for(int i=INDEX_START_BUTTON;i<NUM_BUTTONS;i++){
    draw_button_wrapper(buttons[i]);
  }

  button_draw(connection,screen,window,(wm.main_w-7*strlen(text))/2,(wm.main_h-16),text);

  text=(char*)"Press ESC key to exit...";
  text_draw(connection,screen,window,10,wm.main_h-10,text);
}



void draw_spinner() {
  text_draw(connection,screen,window,spinner_pos.x,spinner_pos.y,text_message);
}



void draw_info_text() {
  int y=info_text_pos.y;
  char rtt[64];
  snprintf(rtt,64,"max delta-vol silence=%.5f", maximum_sample_delta_volume);
  info_text[1]=rtt;
  for(int i=0;i<(int)info_text.size();i++){
    text_draw(connection,screen,window,info_text_pos.x,y,info_text[i].data());
    y+=info_text_line_height;
  }
}






static xcb_format_t *find_format (xcb_connection_t * c, uint8_t depth, uint8_t bpp)
{
  const xcb_setup_t *setup = xcb_get_setup(c);
  xcb_format_t *fmt = xcb_setup_pixmap_formats(setup);
  xcb_format_t *fmtend = fmt + xcb_setup_pixmap_formats_length(setup);
  for(; fmt != fmtend; ++fmt)
    if((fmt->depth == depth) && (fmt->bits_per_pixel == bpp)) {
      /* printf("fmt %p has pad %d depth %d, bpp %d\n",
         fmt,fmt->scanline_pad, depth,bpp); */
      return fmt;
    }
  return 0;
}



void fillimage(unsigned char *p, int width, int height){
  int i, j;
  for(i=0; i < width; i++){
    for(j=0; j < height; j++){
      *p++=(j*400)>>8; // blue
      *p++=(i*j*32)>>8; // green
      *p++=(i*193)>>8; // red
      p++; /* unused byte */
    }
  }
}

#define ankh_depth 8
#define ankh_red_offset 64
#define ankh_grn_offset 64
#define ankh_blu_offset 64

u32 get_bit_at_XY_coords_of_1bit_2D_data_structure(u8* ptr,u32 x,u32 y,u32 width,u32 height){
  u32 old_width=width;
  width=(width+7)&0xfffffff8;
  u32 ds_x=x;
  u32 ds_y=y;
  while(ds_x>=old_width){
    ds_x-=old_width;
  }
  while(ds_y>=height){
    ds_y-=height;
  }
  u32 ankh_index=ds_x+ds_y*width;
  u8 ankh_bit=ankh_index&7;
  u32 ankh_bit_index=ankh_index>>3;
  return (ptr[ankh_bit_index]&(1<<ankh_bit))?1:0;
}

void fillimage_ankh(unsigned char *p, int width, int height){
  int i, j;
  //  u32 ankh_index=0;
  for(j=0; j < height; j++){
    for(i=0; i < width; i++){
      u8 ankh_col=ankh_depth*get_bit_at_XY_coords_of_1bit_2D_data_structure(Ankh_bits,i,j,Ankh_width,Ankh_height);
      u8 ankh_red=ankh_red_offset+ankh_col;
      u8 ankh_grn=ankh_grn_offset+ankh_col;
      u8 ankh_blu=ankh_blu_offset+ankh_col;
      *p++=ankh_blu; // blue
      *p++=ankh_grn; // green
      *p++=ankh_red; // red
      p++; /* unused byte */
    }
  }
}



void fillimage_from_24_bit_rgb(u8 *dest, u8* src,int width, int height){
  int i, j;
  for(i=0; i < width; i++){
    for(j=0; j < height; j++){
      *dest++=*src++; // blue
      *dest++=*src++; // green
      *dest++=*src++; // red
      *dest++=0; /* unused byte */
      src++;
    }
  }
}



xcb_image_t *create_true_colour_image(xcb_connection_t *c, int width, int height){
  const xcb_setup_t *setup = xcb_get_setup(c);
  u8 *image32;
  image32=new u8[width*height*4];
  xcb_format_t *fmt = find_format(c, 24, 32);
  if (fmt == NULL)
    return NULL;

  //  fillimage(image32, width, height);
  fillimage_ankh(image32, width, height);

  xcb_image_order_t image_byte_order=(xcb_image_order_t)setup->image_byte_order;
  xcb_image_order_t image_bit_order=(xcb_image_order_t)0;
  
  return xcb_image_create(width,
                          height,
                          XCB_IMAGE_FORMAT_Z_PIXMAP,
                          fmt->scanline_pad,
                          fmt->depth,
                          fmt->bits_per_pixel,
                          image_bit_order,
                          image_byte_order,
                          XCB_IMAGE_ORDER_LSB_FIRST,
                          image32,
                          width*height*4,
                          image32);
  delete[] image32;
}



xcb_image_t *create_empty_true_colour_image(xcb_connection_t *c, int width, int height){
  const xcb_setup_t *setup = xcb_get_setup(c);
  u8 *image32;
  image32=new u8[width*height*4];
  xcb_format_t *fmt = find_format(c, 24, 32);
  if (fmt == NULL)
    return NULL;

  xcb_image_order_t image_byte_order=(xcb_image_order_t)setup->image_byte_order;
  xcb_image_order_t image_bit_order=(xcb_image_order_t)0;
  
  return xcb_image_create(width,
                          height,
                          XCB_IMAGE_FORMAT_Z_PIXMAP,
                          fmt->scanline_pad,
                          fmt->depth,
                          fmt->bits_per_pixel,
                          image_bit_order,
                          image_byte_order,
                          XCB_IMAGE_ORDER_LSB_FIRST,
                          image32,
                          width*height*4,
                          image32);
  delete[] image32;
}




xcb_image_t *create_true_colour_image_from_rgb_data(xcb_connection_t *c, int width, int height, u8* data){
  const xcb_setup_t *setup = xcb_get_setup(c);
  u8 *image32;
  image32=new u8[width*height*4];
  xcb_format_t *fmt = find_format(c, 24, 32);
  if (fmt == NULL)
    return NULL;

  fillimage_from_24_bit_rgb(image32, data, width, height);

  //  memset(image32,width*height*4,0);
  //  for(int x=0;x<width;x+=10){
  //    for(int y=0;y<height;y++){
  //      u8* p=image32+4*(x+y*width);
  //      *p++=255;
  //      *p++=255;
  //      *p++=255;
  //      *p++=255;
  //    }
  //  }

  xcb_image_order_t image_byte_order=(xcb_image_order_t)setup->image_byte_order;
  xcb_image_order_t image_bit_order=(xcb_image_order_t)0;
  
  return xcb_image_create(width,
                          height,
                          XCB_IMAGE_FORMAT_Z_PIXMAP,
                          fmt->scanline_pad,
                          fmt->depth,
                          fmt->bits_per_pixel,
                          image_bit_order,
                          image_byte_order,
                          XCB_IMAGE_ORDER_LSB_FIRST,
                          image32,
                          width*height*4,
                          image32);

  delete[] image32;
}
