# CSCI-ECEN 4593 Computer Organization Project  
Weipeng Cao, Theerarun Tubnonghee (Steve)  

This project is to Simulate the pipeline of MIPS ISA with hierachical memories. The pipeline consists of five stages: instruction fetch, instruction decode, execution, memory, and writeback. The memories includes independent direct-mapped caches of instruction and data and main memory.

Pipeline Stages: IF, ID, EX, MEM, WB  
Caches: Direct-mapped, dynamical sizing  
Memory: fixed size  
  
Directory
---------
    .
    +--lib/            (optional by Makefile)
    +--bin/            (required by Makefile)
    +--include/        all headers
    +--src/            all source code

## Instruction
### Build
$ make

### Run
$ ./bin/emulator

## Configuration
Memory Size: static.h  
Cache Size: main.c  
Penalties: cache.h, memory.h  
Writebuffer Size: not available  

### Result
Program 1: 
CPI: 1.09
Cycle: 695430

Program 1: 
CPI: 1.00
Cycle: 15594

program [Program1File.txt]
icache	 dcache	       	       	      	       	        Total  
 size 	  size 	 block 	 WT(0) 	i-hit 	 d-hit 	  CPI   clock  
(byte)	 (byte)	(lines)	 WB(1) 	rate(%)	rate(%)	       cycles  
  128	  256	   16	    0	99.71	82.20	 1.30	825133  
  128	  256	   16	    1	99.20	85.00	 1.32	839947  
  128	  256	    4	    0	99.81	79.87	 1.32	838966  
  128	  256	    4	    1	99.65	93.96	 1.26	803089  
  128	  256	    1	    0	99.85	50.70	 1.82	1160909  
  128	  256	    1	    1	99.82	81.98	 1.58	1007938  
   64	 1024	   16	    0	89.38	92.09	 2.93	1868120  
   64	 1024	   16	    1	90.83	87.51	 2.58	1645561  
   64	 1024	    4	    0	98.54	98.58	 1.18	750748  
   64	 1024	    4	    1	99.22	99.22	 1.17	744775  
   64	 1024	    1	    0	97.66	97.04	 1.27	806206  
   64	 1024	    1	    1	98.19	97.36	 1.26	801661  


program [Program2File.txt]
icache	 dcache	       	       	      	       	        Total  
 size 	  size 	 block 	 WT(0) 	i-hit 	 d-hit 	  CPI   clock  
(byte)	 (byte)	(lines)	 WB(1) 	rate(%)	rate(%)	       cycles  
   64	  512	   16	    0	54.03	73.27	 4.94	 76923  
   64	  512	   16	    1	64.78	41.35	 4.81	 75166  
   64	  512	    4	    0	76.07	69.53	 2.80	 43581  
   64	  512	    4	    1	80.70	69.65	 2.72	 42451  
   64	  512	    1	    0	78.16	53.96	 3.49	 54361  
   64	  512	    1	    1	80.02	64.49	 3.41	 53162  
  128	  256	   16	    0	78.55	8.55	 5.20	 80991  
  128	  256	   16	    1	74.22	7.79	 6.66	104172  
  128	  256	    4	    0	99.53	47.71	 1.27	 19720  
  128	  256	    4	    1	99.32	58.09	 1.29	 20170  
  128	  256	    1	    0	98.97	45.91	 1.37	 21425  
  128	  256	    1	    1	98.50	63.93	 1.33	 20758  
  256	  128	   16	    0	99.62	12.37	 2.56	 39914  
  256	  128	   16	    1	99.71	5.72	 4.13	 64670  
  256	  128	    4	    0	99.67	47.08	 1.26	 19658  
  256	  128	    4	    1	99.29	58.10	 1.30	 20201  
  256	  128	    1	    0	99.14	45.56	 1.36	 21217  
  256	  128	    1	    1	98.66	67.62	 1.32	 20590  
