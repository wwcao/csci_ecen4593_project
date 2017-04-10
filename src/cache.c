#include "static.h"

#ifndef __CACHE_HEADER__
#define __CACHE_HEADER__

#define L1_SIZE 
#define L1_CACHESIZE 3 // 3KB
#define L1_BLOCKSIZE 16 // lines
#define L1_ICACHE_SIZE 128 // bytes
#define L1_DCACHE_SIZE 256 // bytes
#define MISS_PENALTY 8 // clockcycles
#define WRITE_POLICY WT

typedef struct {
  bool valid;
  unsigned int tag;
  unsigned int value;
} Cache;

Cache cacheL1[];
#endif // __CACHE_HEADER__

bool readAddr(Cache cache[], unsigned int addr_index){
	unsigned int tag = get_tag(addr_index);
	unsigned int index = get_index(addr_index);
	int i;
	for(i=0; i<L1_BLOCKSIZE<i++){
		if( i==index && tag == cache[i].tag && cache[i].valid == true ){
			return true
		}
	}

	return false
}

int get_tag(unsigned int addr_index){
 unsigned int tagBit= 32 - (L1_BLOCKSIZE>>2) - 2;
 
}

bool is_hit(Cache cache[], unsigned int *addr_index){

 unsigned int index;
 for(i=0; i< L1_SIZE; i++){
  if(cache[i].value == *addr_index){
   
    return true;
  }
 }

 return false;
}

int set_penalty(bool isHit){

 if(isHit) return 0;
 
 return MISS_PENALTY;
 
}
