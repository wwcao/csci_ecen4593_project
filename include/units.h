

#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

#include "static.h"
#include "utils.h"

extern int test[5];

unsigned int clock;

unsigned int memory[MEMORY_SIZE];

unsigned int icacheBNum;
unsigned int dcacheBNum;
unsigned int cacheBSize;

cache *icache;
cache *dcache;
writebuffer wrbuffer[16];
char missedPenalty;

int register_file[REGISTER_SIZE];

unsigned int PC;

IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

#include "cache.h"

void init_units();
void init_registers();
void init_caches();


void destroyCaches();
#endif
