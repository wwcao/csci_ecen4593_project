#include <stdio.h>
#include "static.h"
#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

unsigned int memory[MEMORY_SIZE];
unsigned int imemory[IMEMORY_SIZE];
unsigned int dmemory[DMEMORY_SIZE];
int register_file[REGISTER_NUM];

unsigned int PC;

IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

void init_units();

#endif
