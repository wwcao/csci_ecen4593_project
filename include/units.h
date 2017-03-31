#include "static.h"
#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

extern unsigned int memory[MEMORY_SIZE];
extern unsigned int imemory[IMEMORY_SIZE];
extern unsigned int dmemory[DMEMORY_SIZE];
extern unsigned int register_file[REGISTER_NUM];

extern unsigned int PC;

extern IFID_Register ifid_reg;
extern IDEX_Register idex_reg;
extern EXMEM_Register exmem_reg;
extern MEMWB_Register memwb_reg;

void init_units();

#endif
