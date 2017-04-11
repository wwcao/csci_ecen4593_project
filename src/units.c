#include "units.h"

void init_units() {
  unsigned int i;
  for(i=0; i<MEMORY_SIZE; i+=4) {
    memory[i] = 0;
    memory[i+1] = 0;
    memory[i+2] = 0;
    memory[i+3] = 0;
  }
  readInstruction(progSources[test[0]]);
  init_registers();
  init_caches();
  missedPenalty = 0;
}

void init_registers() {
  register_file[$sp] = memory[0];
	register_file[$fp] = memory[1];
	PC = memory[5];
}

void init_caches() {
  int line, block;
  int min, diff;
  int curAddr;
  cache *clarge;

  cacheBSize = test[3];
  icacheBNum = (test[1]/4/cacheBSize);
  dcacheBNum = (test[2]/4/cacheBSize);

  memset(&icache, 0, sizeof(cache));
  memset(&dcache, 0, sizeof(cache));

  initial_cacheCtl();


}

