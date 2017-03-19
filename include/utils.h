
#include "memory.h"
#include "static.h"

#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__


unsigned memory[MEMORY_SIZE];

unsigned int readInstruction();
op_format getInsFormat(int);
unsigned getPartNum(int, part_type);
void printInstr(int);

// TODO: Function to initialize all units




void Error(const char*);


#endif
