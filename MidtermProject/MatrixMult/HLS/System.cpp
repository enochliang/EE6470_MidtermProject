#include "System.h"
System::System( sc_module_name n, string input_bmp, string output_bmp ): sc_module( n ), 
	tb("tb"), matrixmult("matrixmult"), clk("clk", CLOCK_PERIOD, SC_NS), rst("rst"), _output_bmp(output_bmp)
{
	
	tb.i_clk(clk);
	tb.o_rst(rst);
	matrixmult.i_clk(clk);
	matrixmult.i_rst(rst);

	tb.o_data(data);
	tb.i_result(result);
	matrixmult.i_data(data);
	matrixmult.o_result(result);

	cout << "SYSTEM OK" << endl;
	
	//tb.get_data_num();
    tb.read_matrix(input_bmp);
	//========================
}

System::~System() {
  tb.write_result(_output_bmp);
}
