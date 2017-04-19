

#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

#include "static.h"
#include "utils.h"

extern int test[5];

int register_file[REGISTER_SIZE];
unsigned int PC;
unsigned int clock;

unsigned int memory[MEMORY_SIZE];
bool MemBusy;

IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

unsigned int icacheBNum;
unsigned int dcacheBNum;
unsigned int cacheBSize;

cache *icache;
cache *dcache;
wr_policy wrPolicy;

writebuffer **wrbuffer;

#include "memory.h"
#include "cache.h"

void init_units();
void init_registers();
void init_memory();
void init_caches();

void destroyCaches();
void destroyWRBuffer();

#endif
