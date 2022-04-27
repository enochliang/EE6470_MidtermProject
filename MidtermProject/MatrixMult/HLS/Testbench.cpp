#include <cstdio>
#include <cstdlib>
using namespace std;

#include "Testbench.h"

Testbench::Testbench(sc_module_name n) : sc_module(n) {
  SC_THREAD(feed_data);
  sensitive << i_clk.pos();
  dont_initialize();
  SC_THREAD(fetch_result);
  sensitive << i_clk.pos();
  dont_initialize();
  
}

Testbench::~Testbench() {
	//cout<< "Max txn time = " << max_txn_time << endl;
	//cout<< "Min txn time = " << min_txn_time << endl;
	//cout<< "Avg txn time = " << total_txn_time/n_txn << endl;
	cout << "Total run time = " << total_run_time << endl;
}

/* void Testbench::get_data_num() {
  FILE *fp_s = NULL;
  fp_s = fopen("../data_num.dat", "rb");
  if (fp_s == NULL) {
    printf("fopen data_num.dat error\n");
  }
  fscanf(fp_s,"%u",&data_num);
  fclose(fp_s);
} */

int Testbench::read_matrix(string infile_name) {
  
  FILE *fp_s = NULL; // source file handler
  
  fp_s = fopen(infile_name.c_str(), "rb");
  if (fp_s == NULL) {
    printf("fopen %s error\n", infile_name.c_str());
    return -1;
  }
  
  
  for( unsigned int n = 0 ; n < 10 ; n++ ){
    //printf("The Weight Matrix Read From Stimulus File:\n");
    for( unsigned int i = 0 ; i < 4 ; i++ ){
      fscanf( fp_s, "%d %d %d %d" , &W[i][0][n], &W[i][1][n], &W[i][2][n], &W[i][3][n]);
      //printf("%d %d %d %d\n", W[i][0][n], W[i][1][n], W[i][2][n], W[i][3][n]);
    }
    //printf("The Input Matrix Read From Stimulus File:\n");
    for( unsigned int i = 0 ; i < 4 ; i++ ){
      fscanf( fp_s, "%d %d %d %d" , &IA[i][0][n], &IA[i][1][n], &IA[i][2][n], &IA[i][3][n]);
      //printf("%d %d %d %d\n", IA[i][0][n], IA[i][1][n], IA[i][2][n], IA[i][3][n]);
    }
  }
  
  fclose(fp_s);

  
  return 0;
}

int Testbench::write_result(string outfile_name) {
  FILE *fp_t = NULL;      // target file handler
  unsigned int file_size; // file size

  fp_t = fopen(outfile_name.c_str(), "wb");
  if (fp_t == NULL) {
    printf("fopen %s error\n", outfile_name.c_str());
    return -1;
  }

  for( unsigned int n = 0 ; n < 10 ; n++ ){
    for( unsigned int i = 0 ; i < 4 ; i++ ){
      for( unsigned int j = 0 ; j < 4 ; j++ ){
        fprintf( fp_t, "%d " , OA[i][j][n]);
      }
      fprintf( fp_t, "\n" );
    }
    fprintf( fp_t, "\n" );
  }

  fclose(fp_t);
  return 0;
}

void Testbench::feed_data() {
	n_txn = 0;
	max_txn_time = SC_ZERO_TIME;
	min_txn_time = SC_ZERO_TIME;
	total_txn_time = SC_ZERO_TIME;

#ifndef NATIVE_SYSTEMC
  o_data.reset();
#endif
	o_rst.write(false);
	wait(5);
	o_rst.write(true);
	wait(1);
	total_start_time = sc_time_stamp();
  lat = sc_time_stamp();

  for( unsigned int n = 0 ; n < 10 ; n++ ){
    lat = sc_time_stamp() - lat;
    cout << "sample " << n << " latency = " << lat << endl;
    lat = sc_time_stamp();
    for( int i = 0 ; i < 4 ; i++ ){
      for( unsigned int j = 0 ; j < 4 ; j++ ){
        data.range(8*j+7,8*j) = W[j][i][n];
      }
      #ifndef NATIVE_SYSTEMC
        o_data.put(data);
      #else
        o_data.write(data);
      #endif
    }
    for( unsigned int i = 0 ; i < 4 ; i++ ){
      for( unsigned int j = 0 ; j < 4 ; j++ ){
        data.range(8*j+7,8*j) = IA[i][j][n];
      }
      #ifndef NATIVE_SYSTEMC
        o_data.put(data);
      #else
        o_data.write(data);
      #endif
    }
  }
}

void Testbench::fetch_result() {
#ifndef NATIVE_SYSTEMC
	i_result.reset();
#endif
	wait(5);
	wait(1);
  //printf("The Output Matrix:\n");
  for( unsigned int n = 0 ; n < 10 ; n++ ){
    for (int i = 0; i < 4 ; i++) {
      #ifndef NATIVE_SYSTEMC
        result = i_result.get();
      #else
        result = i_result.read();
      #endif
      for (int j = 0; j < 4; j++) {
        sc_dt::sc_int<16> tmpO;
        tmpO = result.range(16*j+15,16*j);
        OA[i][j][n] = tmpO;
        //printf("%d ",OA[i][j][n]);
      }
      //printf("\n");
    }
  }
  
	total_run_time = sc_time_stamp() - total_start_time;
  sc_stop();
}
