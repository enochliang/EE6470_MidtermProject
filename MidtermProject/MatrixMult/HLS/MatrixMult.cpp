#include <cmath>
#ifndef NATIVE_SYSTEMC
#include "stratus_hls.h"
#endif
//#include "directives.h"
#include "MatrixMult.h"

Matrix_Mult::Matrix_Mult( sc_module_name n ): sc_module( n )
{
#ifndef NATIVE_SYSTEMC
	HLS_MAP_TO_REG_BANK(W);
	HLS_MAP_TO_REG_BANK(IA);
	HLS_MAP_TO_REG_BANK(OA);
	HLS_FLATTEN_ARRAY(data_wire);
	HLS_FLATTEN_ARRAY(o_data_wire);
#endif
	SC_THREAD( do_mult );
	sensitive << i_clk.pos();
	dont_initialize();
	reset_signal_is(i_rst, false);
#ifndef NATIVE_SYSTEMC
	i_data.clk_rst(i_clk, i_rst);
	o_result.clk_rst(i_clk, i_rst);
#endif
}

Matrix_Mult::~Matrix_Mult() {}


void Matrix_Mult::do_mult() {
	{
		#ifndef NATIVE_SYSTEMC
			HLS_DEFINE_PROTOCOL("main_reset");
			i_data.reset();
			o_result.reset();
		#endif
		wait();//Why we wait here?
	}
	while (true) {
		//PIPELINE;
		//input_weight
		//HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
		/* #ifndef NATIVE_SYSTEMC
			//HLS_CONSTRAIN_LATENCY(0, 2, "input_weight");
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				HLS_CONSTRAIN_LATENCY("input_weight");
				HLS_DEFINE_PROTOCOL("input");
				data_wire[i] = i_data.get();
			}
			wait();
		#else
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				data_wire[i] = i_data.read();
			}
		#endif */

		#ifndef NATIVE_SYSTEMC
			//HLS_CONSTRAIN_LATENCY(0, 2, "input_weight");
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				HLS_CONSTRAIN_LATENCY("input_weight");
				HLS_DEFINE_PROTOCOL("input");
				data_wire[i] = i_data.get();
			}
			wait();
			
		#else
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				data_wire[i] = i_data.read();
			}
		#endif


		for( unsigned int i = 0 ; i < 4 ; i++ ){
			#ifndef NATIVE_SYSTEMC
			HLS_CONSTRAIN_LATENCY("lat01");
			#endif
			for( unsigned int k = 0 ; k < 4 ; k++ ){
				#ifndef NATIVE_SYSTEMC
				HLS_UNROLL_LOOP( ON, "get_weight" );
				#endif
				W[k][i] = data_wire[i].range((8*k)+7,8*k);
			}
		}

		{
		//HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
		//input_activation
		#ifndef NATIVE_SYSTEMC
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				HLS_CONSTRAIN_LATENCY("input_ia");
				HLS_DEFINE_PROTOCOL("input");
				data_wire[i] = i_data.get();
			}
			wait();
		#else
			for( unsigned int i = 0 ; i < 4 ; i++ ){
				data_wire[i] = i_data.read();
			}
		#endif

		
		for( unsigned int i = 0 ; i < 4 ; i++ ){
			#ifndef NATIVE_SYSTEMC
			HLS_CONSTRAIN_LATENCY("lat_02");
			#endif
			for( unsigned int k = 0 ; k < 4 ; k++ ){
				#ifndef NATIVE_SYSTEMC
				HLS_UNROLL_LOOP( ON, "get_ia" );
				#endif
				IA[k][i] = data_wire[i].range((8*k)+7,8*k);
			}
		}

		//COMPUTE BLOCK
		/* for (unsigned int i = 0; i < 4 ; ++i ) {
			HLS_CONSTRAIN_LATENCY("lat_c");
			HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
			for(unsigned int j = 0; j < 4 ; ++j ){
				#ifndef NATIVE_SYSTEMC
				HLS_UNROLL_LOOP( ON, "compute" );
				#endif
				OA_row.range(16*j+15,16*j)[i] = (sc_dt::sc_int<16>)(((  (sc_dt::sc_int<8>)(IA_col[i].range(7,0))  *  (sc_dt::sc_int<8>)(W_col[j].range(7,0))  ) + 
				                                                      (  (sc_dt::sc_int<8>)(IA_col[i].range(15,8)) *  (sc_dt::sc_int<8>)(W_col[j].range(15,8)) )) + 
				                                                     ((  (sc_dt::sc_int<8>)(IA_col[i].range(23,16))*  (sc_dt::sc_int<8>)(W_col[j].range(23,16))) + 
						                                              (  (sc_dt::sc_int<8>)(IA_col[i].range(31,24))*  (sc_dt::sc_int<8>)(W_col[j].range(31,24)))));
			}
		} */
		for (unsigned int i = 0; i < 4 ; ++i ) {
			for(unsigned int j = 0; j < 4 ; ++j ){
				//HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
				{
				HLS_CONSTRAIN_LATENCY("lat_r");
				OA[i][j] = 0;
				}
				for(unsigned int n = 0; n < 4 ; ++n ){
					HLS_CONSTRAIN_LATENCY("lat_c");
					//HLS_UNROLL_LOOP( ON, "compute" );
					Tmp = IA[n][i]*W[n][j];
					OA[i][j] += Tmp;
				}
				/* OA[i][j] = ((IA[0][i]*W[0][j] + IA[1][i]*W[1][j]) + (IA[2][i]*W[2][j] + IA[3][i]*W[3][j])); */
			}
		}
		}



















		//OUTPUT BLOCK
		#ifndef NATIVE_SYSTEMC
			for (unsigned int i = 0; i < 4 ; ++i ) {
				HLS_CONSTRAIN_LATENCY("lat_o");
				for (unsigned int j = 0; j < 4 ; ++j ) {
					HLS_UNROLL_LOOP( ON, "owire" );
					o_data_wire[i].range(16*j+15,16*j)=OA[i][j];
				}
			}
		#else
			for (unsigned int i = 0; i < 4 ; ++i ) {
				for (unsigned int j = 0; j < 4 ; ++j ) {
					o_data_wire[i].range(16*j+15,16*j)=OA[i][j];
				}
			}
		#endif
		
		
		#ifndef NATIVE_SYSTEMC
			for (unsigned int i = 0; i < 4 ; ++i ) {
				HLS_DEFINE_PROTOCOL("output");
				o_result.put(o_data_wire[i]);
			}
			wait();
		#else
			for (unsigned int i = 0; i < 4 ; ++i ) {
				o_result.write(o_data_wire[i]);
			}
		#endif


	}
}
