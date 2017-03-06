
#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

#include "memory.h"
#include "static.h"
int memory[MEMORY_SIZE];


int readInstruction();
op_format getInsFormat(int);
unsigned getRegNum(int, reg_type);
void printInstr(int);





void Error(const char*);


#endif
