

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
extern bool CacheEnabled;
extern bool PreCached;
extern int **fileTests;
extern int** progResults;

// globals
count_type insType;
stat_result *results;
unsigned int numTest;
unsigned int numIns;
unsigned int insCounter;
unsigned int numBranch;
unsigned int numLWSW;
unsigned int numR_f;
unsigned int numI_f;
unsigned int numNop;
unsigned int numStall;
unsigned int numHarzard;
unsigned int numRead_I;
unsigned int numRead_D;
unsigned int numReadMissed_I;
unsigned int numReadMissed_D;
unsigned int numWrite_D;
unsigned int numWriteMissed_D;
float cpi;

unsigned int readInstruction(const char* path);
int loadInstructions();
op_format getInsFormat(int);
unsigned getPartNum(int, part_type);
void printInstr(int);
void printSummaryHeader(const char* progName);
void printSummary(const char* argName, const char** progNames, unsigned int len);
unsigned int findMinCpi(unsigned int progNum);
void saveResult(int index, int* config);
void printConfig(const char* src, int* config, unsigned int len);
void statPipeline(count_type itype);
void Error(const char*);
bool testResults(unsigned int index, int* config);
cache* createCache(unsigned int blockNum, unsigned int lineNum);
void destroyUnusedCache(cache* target, unsigned int blockNum);
bool findLBits(unsigned int num, unsigned short *bitNum, unsigned int *mask);
unsigned int getWrData(unsigned int cacheData, unsigned int newData, unsigned short offset, lwsw_len wsize);
writebuffer** initWRBuffers(writebuffer** des);
writebuffer* createWRBuffer_WT(unsigned int addr, unsigned int data);
writebuffer* createWRBuffer_WB(cachedata* cacheData, unsigned addr, int lineNum);
void destroyUnusedWRBuffer(writebuffer** target);

void init_results(unsigned int num);
void init_utils();

bool readConfigs(const char* filename, unsigned int* testNum);
void loadProgResults();

#endif

