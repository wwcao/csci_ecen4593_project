# CSCI-ECEN 4593 Computer Organization Project  
Weipeng Cao, Theerarun Tubnonghee (Steve)  

This project is to Simulate the pipeline of MIPS ISA with hierachical memories. The pipeline consists of five stages: instruction fetch, instruction decode, execution, memory, and writeback. The memories includes independent direct-mapped caches of instruction and data and main memory.

Pipeline Stages: IF, ID, EX, MEM, WB  
Caches: Direct-mapped, dynamical sizing  
Memory: fixed size  
  
Directory
---------
    .
    +--lib/            		(optional by Makefile)
    +--bin/            		(required by Makefile)
    +--include/       		all headers
    +--src/            		all source code
	.--Program1File.txt		(required, MIPS assembly)
	.--Program2File.txt		(required, MIPS assembly)

## Instruction
### Build
$ make

### Run
For submission showing min CPI with an arrow in display: 

~~$ ./bin/emulator test_prog1~~

$ ./bin/emulator test_prog2

$ ./bin/emulator test_all


For a specific configuration:
$ ./bin/emulator {config file}


### Configuration File Format
1. Filename (Not Implemented)
* Beginning with \#

2. Configuration
* Beginning with [0-9] 
* Each filed is separated with delimiter ','  
* Fields {program #, icache, dcache, line, wr policy, cache?, precache?}   
```
Ex:
0,128,256,16,0,0,0
- testing assembly in Program1File.txt  
- icache 128 Byte, dcache 256 Byte, 16 lines
- writing policy: write through
- disabled cache
- disabled early caching
```

3. other chars would be ignored
```
Configuration File Example
// program, icache, dcache, line, wr policy, cache?, precache?

//
//,Normal,Tests
//
0,128,256,16,0,0,0
0,64,1024,1,0,1,1
0,64,1024,1,1,1,1
//
1,64,512,16,0,0,0
//
//,min?
//
0,128,1024,16,1,1,1
0,128,1024,8,1,1,1
0,128,1024,4,1,1,1
```

### Other Configuration

Memory Size: static.h  
Cache Size: ~~main.c~~  external input  
Penalties: cache.h, memory.h  
Writebuffer Size: not available (default: 2)
Early fetch: {0|1}, cache.h (default: disabed, not completely function)
Unified Cache: {0|1}, cache.h (not working)

### Result

Details in files 
- ~~test_prog1.csv~~,   
- test_prog2.csv,   
- test_all.csv  
- files with .csv
