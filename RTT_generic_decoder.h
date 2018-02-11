#pragma once

#define DECODER_DEFAULT_NUM_SAMPLES (16*1024)

#define ABSOLUTE_MINIMUM_BPM_TO_DETECT 50
#define COMB_PHASE_BINS (1+(60*44100)/(NUM_BEAT_DETECT_SAMPLES*60))

class cue_points;
class decoder_generic{
public:
  decoder_generic();
  virtual ~decoder_generic();
  s32 decoder_number;
  s32 *decoded_audio;
  virtual s32 init(const char*_filename)=0;
  virtual s32 decode_bit_of_it()=0;
  virtual double get_current_ms()=0;
  virtual float get_total_ms()=0;
  virtual void done_decoding()=0;
  virtual void seek(float normalised_seek_position)=0;
  virtual void seek_ms(float seek_position_ms)=0;
  virtual float get_replay_gain_dB()=0;
  virtual float get_replay_gain()=0;
  virtual void set_replay_gain(float current_displayed_value,float desired_display_value)=0;
  virtual float get_playtime_left()=0;
  virtual string get_track_title(string &artist_album)=0;
  s32 total_samples_decoded;
  enum{
    DECODERSTATE_STOPPED=0,
    DECODERSTATE_PLAYING=1,
    DECODERSTATE_PLAYING_CUE_NOISE=2,
    DECODERSTATE_IS_BEING_CREATED=3,
    DECODERSTATE_IS_BEING_DESTROYED=4
  };

  s32 state;   // 0=stop 1=play 2=pause
};

#define DECK1 0
#define DECK2 1
#define DECODER_PREVIEW 2
#define DECODER_PLAYLIST_PARSING 3


