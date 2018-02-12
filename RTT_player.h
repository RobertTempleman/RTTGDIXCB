#pragma once
#include "RTT_generic_decoder.h"
#include "RTT_wavpack_decoder.h"

#define PLAYER_BUFFER_NUM_SAMPLES (512*1024) 

#define PLAYER_DISPLAY_WIDTH 270
#define PLAYER_DISPLAY_HEIGHT 47


class three_floats{
public:
  three_floats(){};
  three_floats(float _l,float _r,float _t):l(_l),r(_r),t(_t){};
  float l;
  float r;
  float t;
};



class track{
public:
  explicit track(string &filename);
  ~track();
  decoder_generic *decoder;
  three_floats *audio;
  int write_pos;
  int read_pos;
  double current_sample_track_time;
  float track_rms;
  void put_some_more_in_audio_buffer();
  void post_seek_buffer_operations();
  int how_much_in_buffer();
  void check_decoding();
  void get_one(float &l,float &r);
  float get_current_playpos_track_time_time_left();
  float get_current_playpos_ms();
};



#define PLAYBACK_STATE_PAUSED 0
#define PLAYBACK_STATE_PLAYING_DECK1 1
#define PLAYBACK_STATE_MIXING_1_TO_2 2
#define PLAYBACK_STATE_PLAYING_DECK2 3
#define PLAYBACK_STATE_MIXING_2_TO_1 4

class genre{
public:
  explicit genre(string playlist);
  vector<string> playlist;
  vector<string> track_names;
  deque<int> playback_que;
  void get_one(float &l,float &r);
  void check_buffers_and_trackchange();
  void jump_to_previous_track();
  void jump_to_next_track();
  void jump_to_selected_track();
  string& get_next_track_filename();
  void change_displayed_track_in_playlist(int delta);
  void seek_playing_track(int delta);
  void check_seek_playing_track();
  float get_play_pos_normalized();

  void seek(track *deck, int seek_ammount );
  void skip_a_prefix_in_playlist(int direction);
  int track_pos_in_playlist;
  int trackpos_to_display_as_current_playing_track;
  track *deck1;
  track *deck2;
  float fade_fraction;
  float fade_fraction_delta;
  int playstate;
  bool reload_deck1;
  bool reload_deck2;
  int selected_track_pos_in_playlist;
  bool fade_to_next_track;
  bool fade_to_previous_track;
  bool waiting_to_jump_to_selected_track;
  int seek_delta;
  float current_play_pos_normalized;
};




#define PLAYER_SHOW_PLAYLIST_FOR_MILLISECONDS 5000
class RTTplayer{
public:
  RTTplayer();
  void draw(RTTXCB &dc);

  void plot_track_decoder_buffer_status( s32 last_read_pos_track1_wad, s32 xp, s32 yp, s32 write_pos_wad );
  void draw_waveform(track *t,DWORD ti);
  void put_one_to_draw(float&l);
  void draw_waveform_from_put_buffer();
  void MoveToEx(RTTXCB *rttxcb,s32 lx,s32 ly,s32 dummy);
  void LineTo(RTTXCB *rttxcb,s32 lx,s32 ly);
  void SetPixelV(RTTXCB *rttxcb,s32 x,s32 y,COLORREF col);
  //  void MoveToEx(s32 lx,s32 ly);
  //  void LineTo(s32 lx,s32 ly);
  float *override_wave_buffer;
  s32 override_wave_buffer_pos;
  s32 override_wave_buffer_sample_skip;
//  DWORD check_buffers_and_trackchange_time;
  HPEN penwaveform_put_buffer;
  HPEN penplaypos;
  s32 showing_playlist;
  //  HBITMAP render_bitmap;
  RTTXCB *render_bitmap_dc;
  s32 x;
  s32 y;
  s32 width;
  s32 height;
  track *track1;
  track *track2;
  DWORD draw_track1_wave_until;
  DWORD draw_track2_wave_until;
  genre *techno;
  genre *minimal;
  genre *breaks;
  genre *current_genre;

  int last_read_pos_track1_wad;
  int last_read_pos_track2_wad;
  
  void SelectObject(RTTXCB* dc,HPEN &pen);
  HPEN CreatePen(s32 style,s32 width,COLORREF col);
  void SetDCPenColor2(RTTXCB* dc,COLORREF col);
  void SetDCBrushColor2(RTTXCB* dc,COLORREF col);
  void Rectangle(RTTXCB* dc,s32 left,s32 top,s32 right,s32 bottom);
  s32 move_to_ex_x;
  s32 move_to_ex_y;
  COLORREF select_dc_col;
  COLORREF select_dc_brush_col;
};

extern RTTplayer *player;
