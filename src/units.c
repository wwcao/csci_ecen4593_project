#include "units.h"

void init_units() {
  unsigned int i;
  for(i=0; i<MEMORY_SIZE; i+=4) {
    memory[i] = 0;
    memory[i+1] = 0;
    memory[i+2] = 0;
    memory[i+3] = 0;
  }
  loadInstructions();
  init_registers();
  init_caches();
}

void init_registers() {
  register_file[$sp] = memory[0];
	register_file[$fp] = memory[1];
	PC = memory[5];
}

void init_caches() {
  int i, j;
  cacheBSize = test[3];
  icacheBNum = (test[1]/4/cacheBSize);
  dcacheBNum = (test[2]/4/cacheBSize);

  for(i = 0; i < 128; i++) {
    for(j = 0; j < 128; j++) {
      memset(icache, 0, sizeof(cache));
      memset(dcache, 0, sizeof(cache));
    }
  }
}
