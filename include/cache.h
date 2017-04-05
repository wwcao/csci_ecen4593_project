
#include "static.h"

#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#define   L1_SIZE           0
#define   MISS_PENALTY      8           // clock cycle


typedef struct {
  bool valid;
  unsigned int tag;
  unsigned int data;
} Cache;


#endif // __CACHE_HEADER__
