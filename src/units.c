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

  cachePenalty = 0;
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
    data = 0;
    fillCache(CACHE_I, addr);
    readFromCache(CACHE_I, addr, &data);
    if(data!=memory[addr])
      printf("Asdf");
    addr++;
  }

  addr = 0;
  while (addr < dWordNum) {
    data = 0;
    fillCache(CACHE_D, addr);
    readFromCache(CACHE_D, addr, &data);
    if(data!=memory[addr])
      printf("Asdf");
    addr++;
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
  readInstruction(progSources[test[0]]);
  init_memoryCtl();
}

void destroyCaches() {
  destroyUnusedCache(icache, icacheBNum);
  destroyUnusedCache(dcache, dcacheBNum);
  icache = NULL;
  dcache = NULL;
}

