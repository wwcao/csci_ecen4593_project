
#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#include "static.h"
#include "utils.h"
#include "units.h" //
#include <stdlib.h>
#include <string.h>

#define EARLYFETCH             0
#define UNIFIEDCACHE           0

#define MISS_PENALTY           8           // clock cycle
#define SUBLINE_PENALTY        2

int cacheMissed;

unsigned short cachePenalty;

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

bool readDataCache(unsigned int addr, unsigned int *data);
bool readInsCache(unsigned int addr, unsigned int *data);
void earlyfetch(unsigned int addr);
bool readFromCache(cache_t type, unsigned int addr, unsigned int *data);
bool writeToCache(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize);
void updateCache(unsigned int addr, unsigned int data);
bool writebackCache(cache srcCache, unsigned int block);
bool handleWRCDisabled(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize);
void fillCache(cache_t type, unsigned int addr, bool Subline);
void convertAddr(cache_t ctyp, unsigned int *addr,
                 unsigned int *tag, unsigned int *block, unsigned int *line);

void startCaching();
void cacheInstruction();
void cacheData();

void initial_cacheCtl();
bool checkCache();
#endif // __CACHE_HEADER__
