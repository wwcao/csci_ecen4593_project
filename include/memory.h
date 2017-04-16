#ifndef __MEMORY_HEADER__
#define __MEMORY_HEADER__

#include "static.h"
#include "utils.h"
#include "units.h"

#define MEM_MISS_PENALTY           8           // clock cycle
#define MEM_SUBLINE_PENALTY        2

unsigned int opAddr_mem;
unsigned int opLine_mem;
unsigned short memoryPenalty;

void updateMemory();

void init_memoryCtl();
#endif // __CACHE_HEADER__
