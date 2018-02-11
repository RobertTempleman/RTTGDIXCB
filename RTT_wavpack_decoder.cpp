#include "OS_interface.h"
#include "RTT_generic_decoder.h"
#include "RTT_wavpack_decoder.h"

#define MS_PER_SAMPLE (1000.0f/44100.0f)



track_file_data::track_file_data(const char*_path){
  path=string(_path);
  extract_track_info();
}



track_file_data::track_file_data(string&_path){
  path=_path;
  extract_track_info();
}



void track_file_data::extract_track_info(){
  int n=(int)path.size();
  for(int i=n;i>=0;i--){
    if (path.data()[i]=='\\'){
      char ultra_mega_jesus[1024];
      strcpy(ultra_mega_jesus,&path.data()[i+1]);
      filename=string(ultra_mega_jesus);
      return;
    }
  }
}



void track_file_data::dump_yourself(ofstream&f){
  if (artist==""){
    title=filename;
    artist_album=filename;
  }else{
    artist_album=artist+" - "+album;
  }
  f<<setprecision(10)<<bpm<<endl;
  f<<track<<endl;
  f<<album<<endl;
  f<<artist<<endl;
  f<<composer<<endl;
  f<<year<<endl;
  f<<genre<<endl;
  f<<original_artist<<endl;
  f<<title<<endl;
  f<<artist_album<<endl;
  f<<path<<endl;
  f<<filename<<endl;
  f<<setprecision(10)<<length<<endl;
}



void track_file_data::load_yourself(ifstream &f){
  string data;
  getline(f,data);
  sscanf(data.data(),"%f",&bpm);
  getline(f,track);
  getline(f,album);
  getline(f,artist);
  getline(f,composer);
  getline(f,year);
  getline(f,genre);
  getline(f,original_artist);
  getline(f,title);
  getline(f,artist_album);
  getline(f,path);
  getline(f,filename);
  getline(f,data);
  if (data.size()){
    sscanf(data.data(),"%f",&length);
  }
}



void decoder_wavpack::load_replay_gain(const char*_filename){
  char eror[256];
  WavpackContext *cuntext=WavpackOpenFileInput(_filename,eror,OPEN_TAGS,0);
  if (cuntext){
    load_tag(cuntext);
    int num=WavpackGetNumTagItems(cuntext);
    char tag[1024];
    char value[1024];
    for(int i=0;i<num;i++){
      int anint=WavpackGetTagItemIndexed(cuntext,i,tag,1024);
      int anunt=WavpackGetTagItem(cuntext,tag,value,1024);
      if (strcasecmp(tag,"replaygain_track_gain")==0){
        sscanf(value,"%f",&replay_gain_dB);
        replay_gain=powf(10.0f,0.1f*replay_gain_dB);  
      }
    }
    WavpackCloseFile(cuntext);
  }
}



float decoder_wavpack::get_replay_gain_dB(){
  return replay_gain_dB;
}



float decoder_wavpack::get_replay_gain(){
  return replay_gain;
}



void decoder_wavpack::set_replay_gain(float current_displayed_value,float desired_display_value){
  replay_gain*=sqrtf(desired_display_value/current_displayed_value);
  replay_gain_dB=10.0f*logf(replay_gain);
}



void decoder_wavpack::get_tag_data(string &_filename,track_file_data &data){
  char eror[256];
  WavpackContext *cuntext=WavpackOpenFileInput(_filename.data(),eror,OPEN_TAGS,0);
  if (cuntext){
    load_tag(cuntext);
    int num=WavpackGetNumTagItems(cuntext);
    char tag[1024];
    char value[1024];
    for(int i=0;i<num;i++){
      int anint=WavpackGetTagItemIndexed(cuntext,i,tag,1024);
      int anunt=WavpackGetTagItem(cuntext,tag,value,1024);
      if (strcasecmp(tag,"replaygain_track_gain")==0){
        sscanf(value,"%f",&data.replay_gain);
      }
      if (strcasecmp(tag,"Track")==0){
        data.track=value;
      }
      if (strcasecmp(tag,"Album")==0){
        data.album=value;
      }
      if (strcasecmp(tag,"Artist")==0){
        data.artist=value;
      }
      if (strcasecmp(tag,"Composer")==0){
        data.composer=value;
      }
      if (strcasecmp(tag,"Year")==0){
        data.year=value;
      }
      if (strcasecmp(tag,"Genre")==0){
        data.genre=value;
      }
      if (strcasecmp(tag,"Original artist")==0){
        data.original_artist=value;
      }
      if (strcasecmp(tag,"Title")==0){
        data.title=value;
      }
    }
    const char*p=_filename.data()+_filename.size();
    while(p--!=_filename.data()){
      if (*p=='\\'){
        p++;
        break;
      }
    }
    data.filename=p;

    data.length=(float)WavpackGetNumSamples(cuntext)*MS_PER_SAMPLE;
    WavpackCloseFile(cuntext);
  }
}



decoder_wavpack::decoder_wavpack():decoder_generic(){
  replay_gain=1.0f;
  replay_gain_dB=0.0f;
  cuntext=0;
};



#define NO_EH_ROAR 0
#define AN_EH_ROAR 1

int decoder_wavpack::init(const char*_filename){
  total_samples_decoded=0;
  done_decoding();
  load_replay_gain(_filename);
  filename=string(_filename);
  char eror[256];
  DWORD flags=0;
  cuntext=WavpackOpenFileInput(filename.data(),eror,flags,0);
  if (!cuntext){
    return AN_EH_ROAR;
  }
  int num_channels=WavpackGetNumChannels(cuntext);
  int mode=WavpackGetMode(cuntext);
  int rate=WavpackGetSampleRate(cuntext);
  int bps=WavpackGetBytesPerSample(cuntext);
  if (mode&MODE_FLOAT){
    return AN_EH_ROAR;
  }
  if (num_channels!=2){
    return AN_EH_ROAR;
  }
  if (!cuntext){
    return AN_EH_ROAR;
  }
  if (rate!=44100){
    return AN_EH_ROAR;
  }
  if (bps!=2){
    return AN_EH_ROAR;
  }
  state=decoder_generic::DECODERSTATE_STOPPED;
  return NO_EH_ROAR;
}



int decoder_wavpack::decode_bit_of_it(){
  if (!cuntext) return 0;
  int num=WavpackUnpackSamples(cuntext,decoded_audio,DECODER_DEFAULT_NUM_SAMPLES);
  return num;
}



decoder_wavpack::~decoder_wavpack(){
  if (cuntext){
    WavpackCloseFile(cuntext);
  }
}



double decoder_wavpack::get_current_ms(){
  if (!cuntext) return 0;
  return (double)WavpackGetSampleIndex(cuntext)/44.1;
}



float decoder_wavpack::get_total_ms(){
  if (!cuntext) return 0;
  return (float)WavpackGetNumSamples(cuntext)/44.1f;
}



void decoder_wavpack::done_decoding(){
  if (cuntext){
    WavpackCloseFile(cuntext);
    cuntext=0;
  }
}



void decoder_wavpack::seek(float normalised_seek_position){
  if (!cuntext){
    return;
  }
  int nsamps=WavpackGetNumSamples(cuntext);
  int seeky=(int)((float)nsamps*normalised_seek_position+0.49f);
  if (seeky>nsamps || seeky<0){ 
    {
      int rt=1;
    }
  }

  assert(seeky<=nsamps && seeky>=0 );
  WavpackSeekSample(cuntext,seeky);
}



void decoder_wavpack::seek_ms(float seek_position_ms){
  if (!cuntext){
    return;
  }
  int nsamps=WavpackGetNumSamples(cuntext);
  int seeky=(int)(seek_position_ms*44.1f);
  if (seeky>nsamps || seeky<0){ 
    {
      int rt=1;
    }
  }
  assert(seeky<=nsamps && seeky>=0 );
  WavpackSeekSample(cuntext,seeky);
}



float decoder_wavpack::get_playtime_left(){
  return get_total_ms()-(float)get_current_ms();
}



string decoder_wavpack::get_track_title(string &artist_album){
  return string("enjoy your aids");
}
