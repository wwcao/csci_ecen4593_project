#include <cache.h>

bool readFromCache(char type, unsigned int addr, unsigned int *data) {
  if(!CACHE_ENABLED) {
    *data = memory[addr];
    return true;
  }

  // CACHE READ OPERATION


  return false;
}

bool writeToCache(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize) {
  if(!CACHE_ENABLED) {
    return handleWRCDisabled(addr, data, offset, wsize);
  }

  // CACHE WRITE OPERATION

  return false;
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
    default:
      break;
  }
  return false;
}

void policyWriteback() {

}

void policyWritethrough() {

}

void fillCache(cache_t type, unsigned int addr){
  unsigned int blockInd;
  unsigned int lineInd;
  unsigned int tag;
  cache *cache_des;
  cachedata *block_des;

  lineInd = getLine(addr);
  tag = getTag(type, addr);
  blockInd = getBlock(type, addr);
  switch(type) {
    case CACHE_D:
      cache_des = &(dcache[blockInd]);
      break;
    case CACHE_I:
      cache_des = &(icache[blockInd]);
      break;
  }

  cache_des->valid = true;
  block_des = (cache_des->block);
  block_des += lineInd;
  block_des->data = memory[addr];
  block_des->tag = tag;
  return;
}

unsigned int getTag(cache_t ctype, unsigned int addr) {
  int tag;
  if(ctype == CACHE_D) {
    tag = (addr >> (cacheBSize>>1))>>dcacheBBits;
    return tag;
  }
  else {
    tag = (addr >> (cacheBSize>>1))>>icacheBBits;
    return tag;
  }
}

unsigned int getBlock(cache_t ctype, unsigned int addr) {
  if(ctype == CACHE_D) {
    return (addr>>(cacheBSize>>1))&dcacheBMask;
  }
  else {
    return (addr>>(cacheBSize>>1))&icacheBMask;
  }
}

unsigned int getLine(unsigned int addr) {
  return cacheBSize==4?addr&0x3:0;
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

