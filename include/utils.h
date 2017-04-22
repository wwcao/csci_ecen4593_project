

#ifndef __UTILS_HEADER__
#define __UTILS_HEADER__

#include "static.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned int memory[MEMORY_SIZE];
extern int register_file[REGISTER_SIZE];
extern unsigned int clock;
extern unsigned int PC;
extern const char *progSources[128];

// globals
unsigned int numIns;
unsigned int numBranch;
unsigned int numLWSW;
unsigned int numR_I;
unsigned int numNop;

unsigned int numRead_I;
unsigned int numRead_D;
unsigned int numReadMissed_I;
unsigned int numReadMissed_D;

unsigned int numWrite_D;
unsigned int numWriteMissed_D;

unsigned int readInstruction(const char* path);
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
void printCacheStat();
void init_utils();

cache* createCache(unsigned int blockNum, unsigned int lineNum);
void destroyUnusedCache(cache* target, unsigned int blockNum);
bool findLBits(unsigned int num, unsigned short *bitNum, unsigned int *mask);
unsigned int getWrData(unsigned int cacheData, unsigned int newData, unsigned short offset, lwsw_len wsize);

writebuffer** initWRBuffers(writebuffer** des);
writebuffer* createWRBuffer_WT(unsigned int addr, unsigned int data);
writebuffer* createWRBuffer_WB(cachedata* cacheData, unsigned addr, int lineNum);
void destroyUnusedWRBuffer(writebuffer** target);
#endif
