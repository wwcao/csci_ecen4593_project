#include "static.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

extern unsigned int memory[MEMORY_SIZE];
extern int register_file[REGISTER_SIZE];
extern unsigned int clock;
extern unsigned int PC;

// globals
unsigned int numIns;
unsigned int numBranch;
unsigned int numLWSW;
unsigned int numR_I;
unsigned int numNop;
unsigned int numRead;
unsigned int numWrite;
unsigned int numReadMissed;
unsigned int numWriteMissed;

unsigned int readInstruction();
int loadInstructions();
op_format getInsFormat(int);
unsigned getPartNum(int, part_type);
void printInstr(int);

void printRegisters();
void Error(const char*);
void testSum(unsigned int total);
void printSummary();
void printPipelineStat();
void printReadCacheStat();
void printWriteCacheStat();
void init_utils();

#endif
