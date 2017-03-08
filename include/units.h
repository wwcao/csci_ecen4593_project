#include "static.h"
#ifndef __UNITS_HEADER__
#define __UNITS_HEADER__

extern unsigned memory[MEMORY_SIZE];
extern unsigned imemory[IMEMORY_SIZE];
extern unsigned dmemory[DMEMORY_SIZE];
extern unsigned register_file[REGISTER_NUM];

extern IFID_Register ifid_reg;
extern IDEX_Register idex_reg;
extern EXMEM_Register exmem_reg;
extern MEMWB_Register memwb_reg;


#endif
