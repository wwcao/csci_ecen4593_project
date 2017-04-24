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
$ ./bin/emulator {config file}

#### config file format
*^\# used for insert filename *, ONLY 0|1 
*^[0-9] for a config*  

0 - Program1  
1 - Program2  

config start with number and each is separated with delimiter ','  
fields {program #, icache, dcache, line, wr policy, cache?, precache?}   
e.g. ^0,128,256,16,0,0,0$  

## Configuration
Memory Size: static.h  
Cache Size: main.c  
Penalties: cache.h, memory.h  
Writebuffer Size: not available  

### Result
Program 1:  
Instruction: 415146, CPI: 1.09, Cycle: 695430  

Program 2:  
Instruction: 12142, CPI: 1.00, Cycle: 15594  

More in result.txt


