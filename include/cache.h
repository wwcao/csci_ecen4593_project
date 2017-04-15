
#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#include "static.h"
#include "utils.h"
#include "units.h" //
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CACHE_ENABLED          1

#define MISS_PENALTY           8           // clock cycle
#define SUBLINE_PENALTY        2

unsigned int opAddr_icache;
unsigned int opLine_icache;
unsigned short mPenalty_icache;

unsigned int opAddr_dcache;
unsigned int opLine_dcache;
unsigned short mPenalty_dcache;

cache_state icacheState;
unsigned short icacheBBits;
unsigned int icacheBMask;

cache_state dcacheState;
unsigned short dcacheBBits;
unsigned int dcacheBMask;

unsigned short cacheLBits;
unsigned int cacheLMask;

bool readFromCache(cache_t type, unsigned int addr, unsigned int *data);
bool writeToCache(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize);

bool handleWRCDisabled(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize);

void policyWriteback();
void policyWritethrough();

void fillCache(cache_t type, unsigned int addr);
void convertAddr(cache_t ctyp, unsigned int *addr,
                 unsigned int *tag, unsigned int *block, unsigned int *line);
unsigned int getTag(cache_t type, unsigned int addr);
unsigned int getBlock(cache_t ctyp, unsigned int addr);
unsigned int getLine(unsigned int addr);

void startCaching();

void initial_cacheCtl();
#endif // __CACHE_HEADER__
