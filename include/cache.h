#include <stdlib.h>
#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#include "static.h"

#define   ICACHE_SIZE           0
#define   MISS_PENALTY      8           // clock cycle

char missedPenalty;

typedef struct {
  unsigned int tag;
  unsigned int data;
} cachedata;

typedef struct {
  bool valid;
  cachedata **data;
} cache;

cache* createCache(unsigned int size, unsigned int block);


#endif // __CACHE_HEADER__
