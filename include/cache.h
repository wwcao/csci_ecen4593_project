
#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#include "static.h"
#include "utils.h"
#include "units.h" //
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define CACHE_ENABLED          0

#define MISS_PENALTY           8           // clock cycle
#define SUBLINE_PENALTY        2

unsigned short icacheBBits;
unsigned short dcacheBBits;
unsigned int icacheBMask;
unsigned int dcacheBMask;

bool readFromCache(char type, unsigned int addr, unsigned int *data);
bool writeToCache(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize);

bool handleWRCDisabled(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize);

void policyWriteback();
void policyWritethrough();

void fillCache(cache_t type, unsigned int addr);
unsigned int getTag(cache_t type, unsigned int addr);
unsigned int getBlock(cache_t ctyp, unsigned int addr);
unsigned int getLine(unsigned int addr);


void initial_cacheCtl();
#endif // __CACHE_HEADER__
