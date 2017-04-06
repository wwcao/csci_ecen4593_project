#include "cache.h"

cache* createCache(unsigned int size, unsigned int block) {
  return malloc(sizeof(cache));
}
