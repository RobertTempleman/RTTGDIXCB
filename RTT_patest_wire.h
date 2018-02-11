#include "RTT_2.h"
#include "portaudio.h"
#include <vector>
using namespace std;

int initialize_portaudio();
void shutdown_portaudio();
PaError start_full_duplex_audio_streaming(u32 sample_rate,vector<float> &mono_data_buffer,vector<float> &recording_buffer);
PaError stop_full_duplex_audio_streaming();
void create_interleaved_stereo_float_audio_sample_from_mono_float_data(float *interleaved_stereo_float_MLS_data,float *data,u32 num_samples,float attenuation=1.0f);
extern float maximum_sample_delta_volume;
