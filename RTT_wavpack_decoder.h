#pragma once

extern "C" {
#include "wavpack/wavpack_local.h"
}

class track_file_data{
public:
  track_file_data(){bpm=321;length=123456.0f;replay_gain=1.0f;};
  track_file_data(const char*_path);
  track_file_data(string&_path);

  int bpm;
  string track;
  string album;
  string artist;
  string composer;
  string year;
  string genre;
  string original_artist;
  string title;
  string artist_album;
  string path;
  string filename;
  float length;
  float replay_gain;

  void extract_track_info();
  void dump_yourself(ofstream &cuef);
  void load_yourself(ifstream &f);

};



class decoder_wavpack:public decoder_generic{
public:
  decoder_wavpack();
  virtual int init(const char*_filename);
  virtual int decode_bit_of_it();
  virtual ~decoder_wavpack();
  virtual double get_current_ms();
  virtual float get_total_ms();
  virtual void done_decoding();
  virtual void seek(float normalised_seek_position);
  virtual void seek_ms(float seek_position_ms);
  WavpackContext *cuntext;
  virtual string get_track_title(string &artist_album);
  virtual float get_replay_gain_dB();
  virtual float get_replay_gain();
  virtual void set_replay_gain(float current_displayed_value,float desired_display_value);
  virtual float get_playtime_left();
  void load_replay_gain(const char*_filename);


  static void get_tag_data(string &_filename,track_file_data &data);

  string artist_album_name;
  string trackname_tidied;
  string filename;

  float replay_gain_dB;
  float replay_gain;
};
