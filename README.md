#CSCI-ECEN 4593 Computer Organization Project  
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

