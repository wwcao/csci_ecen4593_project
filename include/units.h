#include "static.h"
#include "utils.h"
#include "cache.h"

#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

extern int test[5];

unsigned int clock;

unsigned int memory[MEMORY_SIZE];

unsigned icacheBNum;
unsigned dcacheBNum;
unsigned cacheBSize;

cache *icache;
cache *dcache;

int register_file[REGISTER_SIZE];

unsigned int PC;

IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

void init_units();
void init_registers();
void init_caches();
#endif
