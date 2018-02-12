#ifndef INCLUDED_RTT_2
#define INCLUDED_RTT_2
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <string>
#include <queue>
#include <list>
#include <memory>
#include <csignal>
#include <iostream>
#include <inttypes.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/xcb_util.h>
#include <xcb/xcb_image.h>
#include "types.h"

using namespace std;

//#include <xcb/xcb_atom.h>
//#include <xcb/xcb_icccm.h>

//typedef __uint8_t  u8;
//typedef __int8_t   s8;
//typedef __uint16_t u16;
//typedef __int16_t  s16;
//typedef __uint32_t u32;
//typedef __int32_t  s32;
//typedef __uint64_t u64;
//typedef __int64_t  s64;

// hz
#define MOUSE_UPDATE_RATE 60

class xcb_connection_window_screen{
 public:
  xcb_connection_t *connection;
  xcb_screen_t *screen;
  xcb_window_t *window;};

  
//void RTT_init();
//void RTT_update();
//void RTT_key(s32 dir,s32 key);

typedef enum {
  STATE_STARTED = 0,          /* default state */
  STATE_KEY_PRESSED = 1,      /* key was pressed, show unlock indicator */
  STATE_KEY_ACTIVE = 2,       /* a key was pressed recently, highlight part
                                   of the unlock indicator. */
  STATE_BACKSPACE_ACTIVE = 3  /* backspace was pressed recently, highlight
                                   part of the unlock indicator in red. */
} unlock_state_t;


typedef enum {
  STATE_PAM_IDLE = 0,         /* no PAM interaction at the moment */
  STATE_PAM_VERIFY = 1,       /* currently verifying the password via PAM */
  STATE_PAM_WRONG = 2         /* the password was wrong */
} pam_state_t;

enum BUTTONS_e{
  INDEX_START_BUTTON=0,
  INDEX_SINGLE_PLAY_BUTTON,
  INDEX_SHUTDOWN_AUDIO_BUTTON,
  INDEX_STOP_MLS_PLAYBACK,
  INDEX_DETERMINE_SYSTEM_AUDIO_LATENCY,
  INDEX_SINGLE_MLS_SAMPLE_RUN,
  INDEX_MULTIPLE_MLS_SAMPLE_RUN,
  NUM_BUTTONS
};

void draw_button_stuff();
void draw_stuff();
void draw_spinner();
void draw_info_text();
void check_xcb_conncetion_error();
void handle_button_press(int button_presssed);
void set_animation_function_on_timer(std::function<void()> func);

xcb_image_t *create_true_colour_image(xcb_connection_t *c, int width, int height);
xcb_image_t *create_empty_true_colour_image(xcb_connection_t *c, int width, int height);
xcb_image_t *create_true_colour_image_from_rgb_data(xcb_connection_t *c, int width, int height, u8* data);

void fillimage(unsigned char *p, int width, int height);

void build_keyboard_map(xcb_connection_t *connection);

#define MAX_STRING_LENGTH 64

class button_data{
public:
  button_data(const char *_txt,s32 _x,s32 _y,s32 _w,s32 _h){
    //    u32 slen=strlen(_txt);
    txt=new char[MAX_STRING_LENGTH];
    strncpy(txt,_txt,MAX_STRING_LENGTH);
    x=_x;
    y=_y;
    w=strlen(txt)*7;
    h=_h;
  }
  char *txt;
  s32 x;
  s32 y;
  s32 w;
  s32 h;

  bool check_button(int x_test,int y_test){
    if (x_test>=x && x_test<x+w && y_test>=y-h && y_test<y){
      return true;
    }
    return false;
  }
};

extern bool mls_playback;
#endif

