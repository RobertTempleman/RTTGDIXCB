void init_LFSR_MLS();
void doit();
void shutdown_everything();
void initialize_audio_and_start_streaming();
void start_latency_measurement();

enum AUDIO_MODES{
  AUDIO_MODE_NOTHING=0,
  AUDIO_MODE_LATENCY_TESTING,
  AUDIO_MODE_MLS_OUTPUT
};

enum AUDIO_SUB_MODES{
  AUDIO_SUB_MODE_NOTHING=0,
  AUDIO_SUB_MODE_LATENCY_TESTING_PINGING_OUTPUT_WITH_PULSE=1,
  AUDIO_SUB_MODE_LATENCY_WAITING_FOR_PULSE=2
};

extern bool audio_is_initialized;
extern int audio_mode;
extern int audio_sub_mode;
extern int num_latency_deltas_left;
