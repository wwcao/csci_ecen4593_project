#include <cache.h>

bool readFromCache(cache_t ctype, unsigned int addr, unsigned int *data) {
  unsigned int block, line, tag;
  cache srcCache;

  if(!CACHE_ENABLED) {
    *data = memory[addr];
    return true;
  }

  // ISSUE: need to fill out the cache from the base addr of the cache block
  convertAddr(ctype, &addr, &tag, &block, &line);
  switch(ctype) {
    case CACHE_D:
      srcCache = dcache[block];
      if(dcacheState  && (line >= opLine_dcache)) return false;
      if(srcCache.valid && (srcCache.tag == tag)) break;

      // Cache Missd
      missedPenalty = MISS_PENALTY;
      opAddr_dcache = addr;
      opLine_dcache = 0;
      dcacheState = CSTATE_RD;
      return false;
    case CACHE_I:
      srcCache = icache[block];
      if(icacheState && (line >= opLine_icache)) return false;
      if(srcCache.valid && (srcCache.tag == tag)) break;

      // Cache Missd
      missedPenalty = MISS_PENALTY;
      opAddr_icache = addr;
      opLine_icache = 0;
      icacheState = CSTATE_RD;
      return false;
    default:
      Error("Error: Wrong Cache type");
  }
  *data = srcCache.block[line];
  return true;
}

bool checkCache(unsigned int addr) {
  unsigned int block, line, tag;
  cache srcCache;

  convertAddr(CACHE_D, &addr, &tag, &block, &line);
  srcCache = dcache[block];

  if(dcacheState || (opLine_dcache <= cacheBSize)) return false;
  if(srcCache.valid && srcCache.tag == tag) return true;

  missedPenalty = MISS_PENALTY;
  opAddr_icache = addr;
  opLine_icache = 0;
  icacheState = CSTATE_RD;

  return false;
}

bool writeToCache(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize) {

  if(!CACHE_ENABLED) {
    return handleWRCDisabled(addr, data, offset, wsize);
  }
  // CACHE WRITE OPERATION

  switch(wrPolicy) {
    case POLICY_WB:
      if(!checkCache(addr)) return false;

      // data in cache
      // write to cache
      break;
    case POLICY_WT:
      if(!checkCache(addr)) return false;

      // data in cache
      // set penalty, set memory busy and write to memory

      // write to cache

      break;
  }
  //return false;
  return true;
}

bool handleWRCDisabled(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize) {
  unsigned int shamt;
  unsigned int mask;
  switch(wsize) {
    case DLEN_W:
      memory[addr] = data;
      return true;
    case DLEN_B:
      shamt = (3-offset)*8;
      data = ((data)&0xff)<<shamt;
      mask = 0xff<<shamt;
      memory[addr] = (memory[addr]&(~mask))|data;
      return true;
    case DLEN_HW:
      shamt = (1-offset)*16;
      data = ((data)&0xffff)<<shamt;
      mask = 0xffff<<shamt;
      memory[addr] = (memory[addr]&(~mask))|data;
      return true;
    default:
      break;
  }
  return false;
}

void policyWriteback() {

}

void policyWritethrough() {

}

void fillCache(cache_t ctype, unsigned int addr){
  unsigned int block;
  unsigned int line;
  unsigned int tag;
  cache *cache_des;
  cachedata *block_des;

  convertAddr(ctype, &addr, &tag, &block, &line);
  switch(ctype) {
    case CACHE_D:
      cache_des = &(dcache[block]);
      break;
    case CACHE_I:
      cache_des = &(icache[block]);
      break;
  }

  cache_des->valid = true;
  cache_des->tag = tag;
  block_des = (cache_des->block);
  block_des += line;
  *block_des = memory[addr+line];
  return;
}

void convertAddr(cache_t ctype, unsigned int* addr,
                 unsigned int *tag, unsigned int *block, unsigned int *line) {
  unsigned int _addr = *addr;
  *line = _addr&cacheLMask;
  if(ctype == CACHE_D) {
    *tag = (_addr>>cacheLBits)&(~dcacheBMask);
    *block = (_addr>>cacheLBits)&dcacheBMask;
  }
  else {
    *tag = (_addr>>cacheLBits)&(~icacheBMask);
    *block = (_addr>>cacheLBits)&icacheBMask;
  }
  *addr = _addr&(~cacheLMask);
}

void initial_cacheCtl() {
  unsigned int bitNum;
  unsigned int bNum;
  unsigned int mask;
  bitNum = 0;
  mask = 0;
  bNum = dcacheBNum;
  while(bNum>1) {
    bitNum++;
    mask = (mask<<1)+1;
    bNum >>= 1;
  }
  dcacheBBits = bitNum;
  dcacheBMask = mask;

  bitNum = 0;
  bNum = icacheBNum;
  mask = 0;
  while(bNum>1) {
    bitNum++;
    mask = (mask<<1)+1;
    bNum >>= 1;
  }
  icacheBBits = bitNum;
  icacheBMask = mask;

  findLBits(cacheBSize, &cacheLBits, &cacheLMask);

  icacheState = CSTATE_IDLE;
  dcacheState = CSTATE_IDLE;

  mPenalty_icache = 0;
  mPenalty_icache = 0;
  return;
}

void startCaching() {
  if(!CACHE_ENABLED) return;

  missedPenalty = missedPenalty>0?missedPenalty-1:0;
  mPenalty_dcache = mPenalty_dcache>0?mPenalty_dcache-1:0;
  mPenalty_icache = mPenalty_icache>0?mPenalty_icache-1:0;

  if((!missedPenalty)&&(icacheState)) {
    switch(icacheState) {
      case CSTATE_RD:
        if(cacheBSize == 1) {
          icacheState = CSTATE_IDLE;
        }
        else{
          opLine_icache++;
          icacheState = CSTATE_RD_SUB;
          mPenalty_icache += SUBLINE_PENALTY;
        }
        fillCache(CACHE_I, opAddr_icache++);
        break;
      case CSTATE_RD_SUB:
        if(mPenalty_icache) break;

        fillCache(CACHE_I, opAddr_icache++);
        opLine_icache++;
        if(opLine_icache == cacheBSize) {
          icacheState = CSTATE_IDLE;
          break;
        }
        mPenalty_icache += SUBLINE_PENALTY;
        break;
      default:
        Error("Unexpected Cache State");
        break;
    }
  }

  if((!missedPenalty)&&(dcacheState)) {
    switch(dcacheState) {
      case CSTATE_RD:
        if(cacheBSize == 1) {
          dcacheState = CSTATE_IDLE;
        }
        else{
          opLine_dcache++;
          dcacheState = CSTATE_RD_SUB;
          mPenalty_dcache += SUBLINE_PENALTY;
        }
        fillCache(CACHE_D, opAddr_dcache++);
        break;
      case CSTATE_RD_SUB:
        if(mPenalty_dcache) break;

        fillCache(CACHE_D, opAddr_dcache++);
        opLine_dcache++;
        if(opLine_dcache == cacheBSize) {
          dcacheState = CSTATE_IDLE;
          break;
        }
        mPenalty_dcache += SUBLINE_PENALTY;
        break;
      default:
        Error("Unexpected Cache State");
        break;
    }
  }
  return;
}


/*
#include "static.h"
#include <math.h>

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
  char tag;
  unsigned int value;
} Cache;

unsigned int penalty = 0;

Cache cacheL1[];
#endif // __CACHE_HEADER__

bool readAddr(Cache cache[], char* addr_index){
	char* tag = get_tag(addr_index);
	int index = get_index(addr_index);

	if( tag == cache[index].tag && cache[index].valid == true ){
		return true;
	}
	//updateAddr(cache[],tag,index,addr_index);
	return false;
}

void updateAddr(Cache cache[],char* tag, int index, char* addr_index){

	cache[index].valid = true;
	cache[index].tag = tag;
	cache[index].value = btoi(addr_index);

}

int btoi(char* binary){
	int i;
   	int result = 0;
   	int power = 0;
   	for (i = strlen(binary) - 1; i >= 0; i--) {
    	    int added = (binary[i] - '0') * (int)pow(2, power);
    	   	result += added;
    	   	power++;
   	}
   	return result;
}

char* htob( char* address){
    int i;
   	char *binary = malloc(sizeof(char) * 33);
    for(i = 2; i < strlen(address); i++) {
        switch(address[i]) {
		    case '0': strcat(bin_add,"0000"); break;
            case '1': strcat(bin_add,"0001"); break;
            case '2': strcat(bin_add,"0010"); break;
            case '3': strcat(bin_add,"0011"); break;
            case '4': strcat(bin_add,"0100"); break;
            case '5': strcat(bin_add,"0101"); break;
            case '6': strcat(bin_add,"0110"); break;
            case '7': strcat(bin_add,"0111"); break;
            case '8': strcat(bin_add,"1000"); break;
            case '9': strcat(bin_add,"1001"); break;
            case 'a': strcat(bin_add,"1010"); break;
            case 'b': strcat(bin_add,"1011"); break;
            case 'c': strcat(bin_add,"1100"); break;
            case 'd': strcat(bin_add,"1101"); break;
            case 'e': strcat(bin_add,"1110"); break;
            case 'f': strcat(bin_add,"1111"); break;
        	}
    	}
	strcat(binary,"\0");
    return binary;
}

char* get_tag(char* addr_index){
	unsigned int tagBit= 32 - (L1_BLOCKSIZE>>2) - 2;
   	char *binary = malloc(sizeof(char) * 33);
   	binary = htob(addr_index);
   	char *tag = malloc(sizeof(char)*tagBit);
   	tag = binary[2:tagBit+2];


	return tag;

}

int get_index(unsigned int addr_index){
	unsigned int indexBit= L1_BLOCKSIZE>>2;
 	char *binary = malloc(sizeof(char) * 33);
   	binary = htob(addr_index);
   	char *indexBin = malloc(sizeof(char)*indexBit);
   	indexBin = binary[31-indexBit:31];
   	int index = btoi(indexBin);

	return index;
}

bool isHit(Cache cache[], char* addr_index){
	return readAddr(cache[],addr_index);
}

int set_penalty(bool is_hit){
	if(is_hit) return 0;
	return MISS_PENALTY;
}
*/

