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
  MemBusy = false;

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
  unsigned int addr;
  unsigned int dWordNum;
  unsigned int iWordNum;

  // Setup Cache Info
  iWordNum = test[1]/4;
  dWordNum = test[2]/4;
  cacheBSize = test[3];
  wrPolicy = test[4];

  icacheBNum = (iWordNum/cacheBSize);
  dcacheBNum = (dWordNum/cacheBSize);

  // initalize cache control
  initial_cacheCtl();

  // allocate caches
  icache = createCache(icacheBNum, cacheBSize);
  dcache = createCache(dcacheBNum, cacheBSize);

  // setup caches
  addr = 0;
  while(addr < iWordNum) {
    fillCache(CACHE_I, addr);
    addr++;
  }

  addr = 0;
  while (addr < dWordNum) {
    fillCache(CACHE_D, addr);
    addr++;
  }
  return;
}

void destroyCaches() {
  destroyUnusedCache(icache, icacheBNum);
  destroyUnusedCache(dcache, dcacheBNum);
}

