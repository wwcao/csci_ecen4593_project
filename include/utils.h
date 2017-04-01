
#include "memory.h"
#include "units.h"
#include "static.h"

#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

unsigned int readInstruction();
int loadInstructions();
op_format getInsFormat(int);
unsigned getPartNum(int, part_type);
void printInstr(int);

// TODO: Function to initialize all units
void printRegisters();



void Error(const char*);


#endif
