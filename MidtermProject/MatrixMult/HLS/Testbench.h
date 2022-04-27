#ifndef TESTBENCH_H_
#define TESTBENCH_H_

#include <string>
using namespace std;

#include <systemc>
using namespace sc_core;

#ifndef NATIVE_SYSTEMC
#include <cynw_p2p.h>
#endif


#define WHITE 255
#define BLACK 0
#define THRESHOLD 90

class Testbench : public sc_module {
public:
	sc_in_clk i_clk;
	sc_out < bool >  o_rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<32> >::base_out o_data;
	cynw_p2p< sc_dt::sc_biguint<64> >::base_in i_result;
#else
	sc_fifo_out< sc_dt::sc_uint<32> > o_data;
	sc_fifo_in< sc_dt::sc_biguint<64> > i_result;
#endif

  SC_HAS_PROCESS(Testbench);

  Testbench(sc_module_name n);
  ~Testbench();

  int read_matrix(string infile_name);
  int write_result(string outfile_name);
  

private:
  /* unsigned int input_rgb_raw_data_offset;
  const unsigned int output_rgb_raw_data_offset;
  unsigned int width;
  unsigned int height;
  unsigned int width_bytes;
  unsigned char bits_per_pixel;
  unsigned short bytes_per_pixel;
  unsigned char *source_bitmap;
  unsigned char *target_bitmap; */
  sc_dt::sc_uint<32> data;
  sc_dt::sc_biguint<64> result;
  int IA[4][4][10], W[4][4][10], OA[4][4][10];
  //unsigned int data_num;

	unsigned int n_txn;
	sc_time max_txn_time;
	sc_time min_txn_time;
	sc_time total_txn_time;
	sc_time total_start_time;
	sc_time total_run_time;
  sc_time lat;

  void feed_data();
	void fetch_result();
};
#endif
