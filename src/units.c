#include "units.h"

void init_units() {
  clock = 0;
  init_memory();
  init_registers();
  init_caches();
  wrbuffer = initWRBuffers(wrbuffer);
  return;
}

void init_registers() {

  int i;
  for(i = 0; i < 32; i++) {
    register_file[i] = 0;
    register_file[i+1] = 0;
    register_file[i+2] = 0;
    register_file[i+3] = 0;
  }

  register_file[$sp] = memory[0];
	register_file[$fp] = memory[1];
	PC = memory[5];
}

void init_caches() {
  unsigned int data;
  unsigned int addr;
  unsigned int dWordNum;
  unsigned int iWordNum;

  if(!CacheEnabled) return;

  cachePenalty = 0;
  // Setup Cache Info
  iWordNum = config[1]/4;
  dWordNum = config[2]/4;
  cacheBSize = config[3];
  wrPolicy = config[4];

  icacheBNum = (iWordNum/cacheBSize);
  dcacheBNum = (dWordNum/cacheBSize);

  if(iWordNum<cacheBSize||dWordNum<cacheBSize) Error("Invalid Cache Setting");
  // initalize cache control
  initial_cacheCtl();

  // allocate caches
  icache = createCache(icacheBNum, cacheBSize);
  dcache = createCache(dcacheBNum, cacheBSize);

  // setup caches
  if(PreCached) {
    addr = 0;
    while(addr < iWordNum) {
      data = 0;
      fillCache(CACHE_I, addr, true);
      readFromCache(CACHE_I, addr, &data);
      addr++;
    }

    addr = 0;
    while (addr < dWordNum) {
      data = 0;
      fillCache(CACHE_D, addr, true);
      readFromCache(CACHE_D, addr, &data);
      addr++;
    }
  }
  return;
}

void init_memory() {
  unsigned int i;
  MemBusy = false;
  for(i=0; i<MEMORY_SIZE; i+=4) {
    memory[i] = 0;
    memory[i+1] = 0;
    memory[i+2] = 0;
    memory[i+3] = 0;
  }
  readInstruction(progSources[config[0]]);
  init_memoryCtl();
}

void destroyCaches() {
  destroyUnusedCache(icache, icacheBNum);
  destroyUnusedCache(dcache, dcacheBNum);
  icache = NULL;
  dcache = NULL;
}

void destroyWRBuffer() {
  destroyUnusedWRBuffer(wrbuffer);
}

