#include <vector>
using namespace std;
using std::vector;


class LFSR{
public:
  LFSR(int log2_length,bool random_start=false);
  ~LFSR();
  vector<int> taps;
  vector<float> data;
  vector<u8> data_binary;
  u32 num;
  u32 curval;
  u32 mask;
  u32 lfsr_mask_used;
  u32 log2_length;
  //  void get_it(short *mono_lfsr_sample_buffer,log
};



class lfsr_polynomial_mask{
public:
  lfsr_polynomial_mask(u32 _num,u32 *_data):num(_num),data(_data) {};
  u32 num;
  u32 *data;
};



class walsh_hadamard{
public:
  walsh_hadamard(u32 _order, vector<u8> &_mls);
  u32 order;
  u32 P;
  vector<u8> &mls;
  //  vector<u32> input_permutation;
  //  vector<u32> output_permutation;
  vector<vector<u8> > tagS;
  vector<vector<u8> > tagL;
  void fast_hadamard(vector<float> &x,vector<float> &y);
};
  
