#include "units.h"

void init_units() {
  unsigned int i;
  for(i=0; i<MEMORY_SIZE; i++) {
    memory[i] = 0;
  }
  loadInstructions();
  init_registers();
  init_caches();
}

void init_registers() {
  register_file[$sp] = memory[0];
	register_file[$fp] = memory[1];
	PC = memory[5];
	/*
	printf("Initializing units\n------\n$sp[0x%08x]\t$fp[0x%08x]\nPC[0x%08x]\n------\n",
			register_file[$sp], register_file[$fp], PC);
			*/
}

void init_caches() {
  //icache = createCache(tests[testNum][1],tests[testNum][3]);
  //dcache = createCache(tests[testNum][2],tests[testNum][3]);
  if((!icache)||(!dcache))
    Error("Unable to create caches");

  // filling caches
}
