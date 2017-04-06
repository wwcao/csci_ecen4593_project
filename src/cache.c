#include "cache.h"

cache* createCache(unsigned int block, unsigned int line) {
  int i;
  cache *newCache;
  cachedata *newData;

  newCache = (cache*)malloc(block*sizeof(cache));
  if(!newCache)
    Error("Unable to allocate memory for Cache");

  memset(newCache, 0, block*sizeof(cache));
  for(i = 0; i < block; i++) {
    newData = (cachedata*)malloc(line*sizeof(cachedata));
    if(!newData)
      Error("Unable to allocate memory for Block");

    memset(newData, 0, line*sizeof(cachedata));
    newCache[i].block = newData;
  }
  return newCache;
}

void cacheGC() {

}
