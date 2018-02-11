#include "OS_interface.h"
#include "RTT_generic_decoder.h"

decoder_generic::decoder_generic():state(DECODERSTATE_IS_BEING_CREATED){
  decoded_audio=new int[DECODER_DEFAULT_NUM_SAMPLES*2];
}

decoder_generic::~decoder_generic(){
  delete[] decoded_audio;
}

