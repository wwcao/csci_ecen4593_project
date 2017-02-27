
#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

#include "units.h"
#include "static.h"
int memory[MEMORY_SIZE];


int readInstruction();
op_type getInsType(int);
unsigned getRegNum(int, reg_type);
void printInstr(int);





void Error(const char*);


#endif
