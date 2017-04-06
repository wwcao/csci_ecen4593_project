#include "static.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#define   ICACHE_SIZE           0
#define   MISS_PENALTY      8           // clock cycle

char missedPenalty;

bool readAddr(unsigned int addr, unsigned int data);
bool writeAddr(unsigned int addr, unsigned int data);

void policyWriteback();
void policyWritethrough();
#endif // __CACHE_HEADER__
