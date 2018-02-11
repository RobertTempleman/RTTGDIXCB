/** RTT adapted this full duplex sample to allow for recoding the MLS playback realtime 
*/

/** @file patest_wire.c
  @ingroup test_src
  @brief Pass input directly to output.

  Note that some HW devices, for example many ISA audio cards
  on PCs, do NOT support full duplex! For a PC, you normally need
  a PCI based audio card such as the SBLive.

  @author Phil Burk  http://www.softsynth.com

  While adapting to V19-API, I excluded configs with framesPerCallback=0
  because of an assert in file pa_common/pa_process.c. Pieter, Oct 9, 2003.

  */
/*
 * $Id: patest_wire.c 1843 2012-06-22 21:58:10Z philburk $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however,
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original 1developer so that
 * they can be incorporated into the canonical version. It is also
 * requested that these non-binding requests be included along with the
 * license above.
 */
 
#include <stdio.h>
#include <math.h>
#include "RTT_patest_wire.h"
#include "RTT_2.h"
#include "doit_all.h"

#define GLOBAL_VOLUME_MLS 0.02f   // set the maximum volume output of the audio subsystem
#define GLOBAL_VOLUME_IN 1.5f   // set the maximum volume output of the audio subsystem

#define DELTA_FUNCTION_VOLUME 0.1f

class WireConfig_t {
public:
  WireConfig_t();
  int isInputInterleaved;
  int isOutputInterleaved;
  int numInputChannels;
  int numOutputChannels;
  int framesPerCallback;
  /* count status flags */
  int numInputUnderflows;
  int numInputOverflows;
  int numOutputUnderflows;
  int numOutputOverflows;
  int numPrimingOutputs;
  int numCallbacks;
  // RTTs extra config
  float *mono_data_buffer;
  int sample_pos;
  int num_samples_in_mono_data_buffer;
  float *recorded_stereo_interleaved_audio;
  int record_sample_pos;
  int num_samples_in_recorded_stereo_interleaved_audio;
  int num_repetitions;  // how many times to repeat the MLS sequence
  float *attentuations; // array of volumes for each repetition
  void record_sample(float inf);
};


WireConfig_t::WireConfig_t(){
  record_sample_pos=0;
  sample_pos=0;
}
  
WireConfig_t global_config;


/* Latencies set to defaults. */

#define INPUT_FORMAT  paFloat32
#define OUTPUT_FORMAT  paFloat32

#define INPUT_DEVICE           (Pa_GetDefaultInputDevice())
#define OUTPUT_DEVICE          (Pa_GetDefaultOutputDevice())


void WireConfig_t::record_sample(float inf){
  // just record all sound when idling here
  recorded_stereo_interleaved_audio[record_sample_pos++]=inf;
  if (record_sample_pos>=num_samples_in_recorded_stereo_interleaved_audio){
    record_sample_pos=0;
  }
}


/* This routine will be called by the PortAudio engine when audio is needed.
** It may be called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/

u32 global_finished_playing_buffer=0;
float maximum_sample_delta_volume=0;

static int wireCallback(const void *inputBuffer,void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData) {
  float *in;
  float *out;
  static float last_sample_value=0;
  WireConfig_t *config=(WireConfig_t *)userData;
  unsigned int i;

  /* This may get called with NULL inputBuffer during initial setup. */
  if(inputBuffer==NULL) return 0;

  /* Count flags */
  if((statusFlags & paInputUnderflow)!=0) config->numInputUnderflows+=1;
  if((statusFlags & paInputOverflow)!=0) config->numInputOverflows+=1;
  if((statusFlags & paOutputUnderflow)!=0) config->numOutputUnderflows+=1;
  if((statusFlags & paOutputOverflow)!=0) config->numOutputOverflows+=1;
  if((statusFlags & paPrimingOutput)!=0) config->numPrimingOutputs+=1;
  config->numCallbacks+=1;

  // assumes interleaved samples & 2 float channels in and out
  in=((float*)inputBuffer);
  out=((float*)outputBuffer);

  switch(audio_mode){
    case AUDIO_MODE_NOTHING:
      for(i=0; i<framesPerBuffer; i++) {
        for(int j=0;j<2;j++){
          float inf=*in;
          float outf=inf*GLOBAL_VOLUME_IN;
          if (mls_playback){
            outf+=GLOBAL_VOLUME_MLS*config->mono_data_buffer[config->sample_pos];
          }
          config->record_sample(inf);
          *out=outf;
          out++;
          in++;
        }
        config->sample_pos++;
        if (config->sample_pos>=config->num_samples_in_mono_data_buffer){
          config->sample_pos=0;
          global_finished_playing_buffer++;
        }
      }
      break;
    case AUDIO_MODE_LATENCY_TESTING:
      // determine the latecy by timing how long a delta fn takes to go through the system
      switch(audio_sub_mode){
        case AUDIO_SUB_MODE_LATENCY_TESTING_PINGING_OUTPUT_WITH_PULSE:
          memset(out,0,sizeof(*out)*2*framesPerBuffer);
          *out=DELTA_FUNCTION_VOLUME;
          out++;
          *out=DELTA_FUNCTION_VOLUME;
          audio_sub_mode=AUDIO_SUB_MODE_LATENCY_WAITING_FOR_PULSE;
          maximum_sample_delta_volume=0;
          // get the maximum ambient volume change per sample
          for(i=0; i<framesPerBuffer; i++) {
            float val=fabsf(*(in++));
            val+=fabsf(*(in++));
            if (val>maximum_sample_delta_volume){
              maximum_sample_delta_volume=val;
            }
          }
          break;
        case AUDIO_SUB_MODE_LATENCY_WAITING_FOR_PULSE:
          for(i=0; i<framesPerBuffer; i++) {
            float val=fabsf(*(in++));
            val+=fabsf(*(in++));
          }
	  
          num_latency_deltas_left--;
          if (num_latency_deltas_left>0){
            audio_sub_mode=AUDIO_SUB_MODE_LATENCY_TESTING_PINGING_OUTPUT_WITH_PULSE;
          }else{
            audio_mode=AUDIO_MODE_NOTHING;
          }
          break;
      }
      break;
  }
  return 0;
}



void create_interleaved_stereo_float_audio_sample_from_mono_float_data(float *interleaved_stereo_float_MLS_data,float *data,u32 num_samples,float attenuation){
  for(int i=0;i<(int)num_samples;i++){
    float f=data[i]*attenuation;
    *(interleaved_stereo_float_MLS_data++)=f;
    *(interleaved_stereo_float_MLS_data++)=f;
  }
}



// RTT: adapted main be the audio initialization routine
int initialize_portaudio() {
  PaError err=paNoError;
  WireConfig_t *config=&global_config;
  memset((void*)config,0,sizeof(global_config));

  err=Pa_Initialize();
  if(err!=paNoError) goto error;

  printf("initalizing portaudio\n");
  printf("  input format = %lu\n",INPUT_FORMAT);
  printf("  output format = %lu\n",OUTPUT_FORMAT);
  printf("  input device ID  = %d\n",INPUT_DEVICE);
  printf("  output device ID = %d\n",OUTPUT_DEVICE);

  config->isInputInterleaved=1;
  config->isOutputInterleaved=1;
  config->numInputChannels=2;
  config->numOutputChannels=2;
  config->framesPerCallback=128;
  return 0;

error:
  Pa_Terminate();
  fprintf(stderr,"An error occured while using the portaudio stream\n");
  fprintf(stderr,"Error number: %d\n",err);
  fprintf(stderr,"Error message: %s\n",Pa_GetErrorText(err));
  return -1;
}



void shutdown_portaudio() {
  Pa_Terminate();
  printf("Full duplex sound shutdown.\n"); fflush(stdout);
}


static PaStream *stream;

PaError start_full_duplex_audio_streaming(u32 sample_rate,vector<float> &mono_data_buffer,vector<float> &recording_buffer) {
  WireConfig_t *config=&global_config;
  PaError err=paNoError;
  PaStreamParameters inputParameters,outputParameters;

  printf("input %sinterleaved!\n",(config->isInputInterleaved?" ":"NOT "));
  printf("output %sinterleaved!\n",(config->isOutputInterleaved?" ":"NOT "));
  printf("input channels = %d\n",config->numInputChannels);
  printf("output channels = %d\n",config->numOutputChannels);
  printf("framesPerCallback = %d\n",config->framesPerCallback);

  inputParameters.device=INPUT_DEVICE;              /* default input device */
  if(inputParameters.device==paNoDevice) {
    fprintf(stderr,"Error: No default input device.\n");
    goto error;
  }
  inputParameters.channelCount=config->numInputChannels;
  inputParameters.sampleFormat=INPUT_FORMAT|(config->isInputInterleaved?0:paNonInterleaved);
  inputParameters.suggestedLatency=Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
  inputParameters.hostApiSpecificStreamInfo=NULL;

  outputParameters.device=OUTPUT_DEVICE;            /* default output device */
  if(outputParameters.device==paNoDevice) {
    fprintf(stderr,"Error: No default output device.\n");
    goto error;
  }
  outputParameters.channelCount=config->numOutputChannels;
  outputParameters.sampleFormat=OUTPUT_FORMAT|(config->isOutputInterleaved?0:paNonInterleaved);
  outputParameters.suggestedLatency=Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo=NULL;

  config->numInputUnderflows=0;
  config->numInputOverflows=0;
  config->numOutputUnderflows=0;
  config->numOutputOverflows=0;
  config->numPrimingOutputs=0;
  config->numCallbacks=0;
  config->mono_data_buffer=mono_data_buffer.data();
  config->num_samples_in_mono_data_buffer=mono_data_buffer.size();
  config->recorded_stereo_interleaved_audio=recording_buffer.data();
  config->num_samples_in_recorded_stereo_interleaved_audio=recording_buffer.size();

  err=Pa_OpenStream(&stream,
                      &inputParameters,
                      &outputParameters,
                      sample_rate,
                      config->framesPerCallback, /* frames per buffer */
                      paClipOff, /* we won't output out of range samples so don't bother clipping them */
                      wireCallback,
                      config);
  if(err!=paNoError) goto error;

  err=Pa_StartStream(stream);
  if(err!=paNoError) goto error;
  return paNoError;
error:
  Pa_Terminate();
  fprintf(stderr,"An error occured while using the portaudio stream\n");
  fprintf(stderr,"Error number: %d\n",err);
  fprintf(stderr,"Error message: %s\n",Pa_GetErrorText(err));
  return -1;

}



#define CHECK_FLAG_COUNT(member) \
    if( config->member > 0 ){\
      printf("FLAGS SET: " #member " = %d\n",config->member);\
    }

PaError stop_full_duplex_audio_streaming() {
  WireConfig_t *config=&global_config;
  PaError err=paNoError;
  printf("Closing stream.\n");
  err=Pa_CloseStream(stream);
  if(err!=paNoError) goto error;

  CHECK_FLAG_COUNT(numInputUnderflows);
  CHECK_FLAG_COUNT(numInputOverflows);
  CHECK_FLAG_COUNT(numOutputUnderflows);
  CHECK_FLAG_COUNT(numOutputOverflows);
  CHECK_FLAG_COUNT(numPrimingOutputs);
  printf("number of callbacks = %d\n",config->numCallbacks);

  return 1;

error:
  return err;
}
