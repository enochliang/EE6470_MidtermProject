# **EE6470 Midterm Report**

Student ID : 110061644

My Github repo : https://github.com/enochliang/2022_Spring_NTHUEE_ESL_Design_Synthesis_EE6470.git

- Please choose an application for your implementation. You may also choose any algorithm to implement as long as it can be easily parallelizable (for final project) without a large auxiliary library support. Image processing is not a good choice, since it will be too similar to our homework.

  

  The algorithm I choose is 4*4 Matrix Multiplication.

  

- You must decompose the algorithm into software and hardware parts. The software part will be test benches for the design.

  

  As the repo.

  

- Please decompose the memory transfer into a fixed-sized slice and store it locally in the hardware module. Then implement the hardware module to handle the operations. Please transfer the operation to the kernel hardware via register interface (memory map).

  

  I take 4 cycles to transfer one matrix, and I do it two times for input schedule, and do it one time for output schedule.

  

- Please also synthesize and optimize the hardware module for latency and throughput with HLS.

  

  The result is as the table below.

  

- Please annotate timings from HLS to the hardware TLM module, and please show the latency and throughput of executing test benches.

  

  As the testbench.cpp.

  

- Please compare the latency/throughput/area of different implementations based on different HLS directives (the base comparison is the version without any HLS directive).

This is the computation code.

```C++
//COMPUTE BLOCK
		for (unsigned int i = 0; i < 4 ; ++i ) {
			for(unsigned int j = 0; j < 4 ; ++j ){
				HLS_CONSTRAIN_LATENCY("lat_c");
				//HLS_UNROLL_LOOP( ON, "compute" );
				OA[i][j] = ((IA[0][i]*W[0][j] + IA[1][i]*W[1][j]) + (IA[2][i]*W[2][j] + IA[3][i]*W[3][j]));
			}
		}
```

The table below is the comparison of the design in different constrain.

I unroll the computation for-loop and set the latency constrain and make a comparison of them.

The latency will go down when I unroll the computation loop, the number of iteration is become 1/4 time from the basic version. And when we set the latency constraint down the total latency will go down and the area will go up.

|                   | BASIC        | UNROLL_inner-loop | UNROLL &  Latency=4 | UNROLL &  Latency=3 | UNROLL &  Latency=1 |
| ----------------- | ------------ | ----------------- | ------------------- | ------------------- | ------------------- |
| Total Area        | 14489.4      | 15674             | 17727.5             | 17766.2             | 21397.7             |
| Latency (1  iter) | 1870ns       | 870ns             | 380ns               | 340ns               | 260ns               |
| Cycles (1  iter)  | 187          | 87                | 38                  | 34                  | 26                  |
| Latency_Detail    |              |                   |                     |                     |                     |
| input_W           | 1*(4 iter)   | 1*(4 iter)        | 1*(4 iter)          | 1*(4 iter)          | 1*(4 iter)          |
| lat01             | 1*(4 iter)   | 1*(4 iter)        | 1*(4 iter)          | 1*(4 iter)          | 1*(4 iter)          |
| input_IA          | 1*(4 iter)   | 1*(4 iter)        | 1*(4 iter)          | 1*(4 iter)          | 1*(4 iter)          |
| lat02             | 1*(4 iter)   | 1*(4 iter)        | 1*(4 iter)          | 1*(4 iter)          | 1*(4 iter)          |
| lat_Computation   | 10*(16 iter) | 16*(4 iter)       | 4*(4 iter)          | 3*(4 iter)          | 1*(4 iter)          |
| lat_03            | 1*(4 iter)   | 1*(4 iter)        | 1*(4 iter)          | 1*(4 iter)          | 1*(4 iter)          |
| output_OA         | 1*(4 iter)   | 1*(4 iter)        | 1*(4 iter)          | 1*(4 iter)          | 1*(4 iter)          |
| total             | 184          | 88                | 40                  | 36                  | 24                  |



This is the pipeline part. Because I think the way I write the code in the previous part is too hard to do pipeline, so I change the way to write the code like this code block below.

```c++
//COMPUTE BLOCK
		for (unsigned int i = 0; i < 4 ; ++i ) {
			for(unsigned int j = 0; j < 4 ; ++j ){
				HLS_PIPELINE_LOOP(SOFT_STALL, 1, "Main" );
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
			}
		}
```



|                   | BASIC   | Pipeline on loop  layer 3 | UNROLL_most-inner-loop | Pipeline on  loop layer 2 | Pipeline on  loop layer 1 |
| ----------------- | ------- | ------------------------- | ---------------------- | ------------------------- | ------------------------- |
| Total Area        | 13789.3 | 15365                     | 13868                  | 15523                     | 20951                     |
| Latency (1  iter) | 1710ns  | 1180ns                    | 910ns                  | 380ns                     | 260ns                     |
| Cycles (1  iter)  | 171     | 118                       | 91                     | 38                        | 26                        |



Below is the DPA part. I make some observation of the synthesis result.

If I set so many latency constrains or pipelining constrain, and then do HLS with DPA, the clock cycles will almost be the same, and the area will be smaller.

If I don’t set any constrains, and then do HLS with DPA, the clock cycles maybe decreased, and the area should be larger.

|                   | BASIC   | DPA   | Pipeline on  loop layer 2 | DPA   | Pipeline on  loop layer 1 | DPA   |
| ----------------- | ------- | ----- | ------------------------- | ----- | ------------------------- | ----- |
| Total Area        | 13789.3 | 14272 | 15523                     | 15152 | 20951                     | 19590 |
| Latency (1  iter) | 1710ns  | 860ns | 380ns                     | 380ns | 260ns                     | 260ns |
| Cycles (1  iter)  | 171     | 86    | 38                        | 38    | 26                        | 26    |
