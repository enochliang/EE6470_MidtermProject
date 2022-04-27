#ifndef SYSTEM_H_
#define SYSTEM_H_
#include <systemc>
using namespace sc_core;

#include "Testbench.h"
#ifndef NATIVE_SYSTEMC
#include "MatrixMult_wrap.h"
#else
#include "MatrixMult.h"
#endif

class System: public sc_module
{
public:
	SC_HAS_PROCESS( System );
	System( sc_module_name n, std::string input_bmp, std::string output_bmp );
	~System();
private:
    Testbench tb;
#ifndef NATIVE_SYSTEMC
	Matrix_Mult_wrapper matrixmult;
#else
	Matrix_Mult matrixmult;
#endif
	sc_clock clk;
	sc_signal<bool> rst;
#ifndef NATIVE_SYSTEMC
	cynw_p2p< sc_dt::sc_uint<32> > data;
	cynw_p2p< sc_dt::sc_biguint<64> > result;
#else
	sc_fifo< sc_dt::sc_uint<32> > data;
	sc_fifo< sc_dt::sc_biguint<64> > result;
#endif
	std::string _output_bmp;
};
#endif
