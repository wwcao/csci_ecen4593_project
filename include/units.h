#include <stdio.h>

#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

#include "static.h"
#include "cache.h"
#include "utils.h"

unsigned int memory[MEMORY_SIZE];

cache *icache;
cache *dcache;

int register_file[REGISTER_NUM];

unsigned int PC;

IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

void init_units();
void init_registers();
void init_caches();
#endif
