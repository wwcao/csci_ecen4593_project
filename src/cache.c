#include <cache.h>

bool readDataCache(unsigned int addr, unsigned int *data) {
  unsigned int block, line, tag;
  cache srcCache;

  convertAddr(CACHE_D, &addr, &tag, &block, &line);
  if((cacheBSize==1)&&(dcacheState==CSTATE_RD))
    return false;
  if(dcacheState && (line >= opLine_dcache))
    return false;
  srcCache = dcache[block];
  if(srcCache.valid && (srcCache.tag == tag)) {
    *data = srcCache.block[line];
    return true;
  }
  if(dcacheState||icacheState||MemBusy)
    return false;

  mPenalty_dcache = MISS_PENALTY;
  opAddr_dcache = addr;
  opLine_dcache = cacheBSize>1?0:1;
  dcacheState = CSTATE_RD;
  return false;
}

bool readInsCache(unsigned int addr, unsigned int *data) {
  unsigned int block, line, tag;
  cache srcCache;

  convertAddr(CACHE_I, &addr, &tag, &block, &line);
  srcCache = icache[block];

  if((cacheBSize==1)&&(icacheState==CSTATE_RD))
    return false;
  if(icacheState && (line >= opLine_icache))
    return false;
  if(srcCache.valid && (srcCache.tag == tag)) {
    *data = srcCache.block[line];
    return true;
  }
  if(icacheState||dcacheState || MemBusy)
    return false;

  mPenalty_icache = MISS_PENALTY;
  opAddr_icache = addr;
  opLine_icache = cacheBSize>1?0:1;
  icacheState = CSTATE_RD;

  return false;
}

bool readFromCache(cache_t ctype, unsigned int addr, unsigned int *data) {
  if(!CACHE_ENABLED) {
    *data = memory[addr];
    return true;
  }

  if(ctype == CACHE_I) {
    return readInsCache(addr, data);
  }
  else {
    return readDataCache(addr, data);
  }
}

/*
bool readFromCache(cache_t ctype, unsigned int addr, unsigned int *data) {

  unsigned int block, line, tag;
  cache srcCache;

  if(!CACHE_ENABLED) {
    *data = memory[addr];
    return true;
  }

  convertAddr(ctype, &addr, &tag, &block, &line);
  switch(ctype) {
    case CACHE_D:
      if((cacheBSize==1)&&(dcacheState==CSTATE_RD))
        return false;
      if(dcacheState && (line >= opLine_dcache))
        return false;
      srcCache = dcache[block];
      if(srcCache.valid && (srcCache.tag == tag))
        break;
      if(mPenalty_dcache > 0)
        return false;

      mPenalty_dcache = MISS_PENALTY;
      opAddr_dcache = addr;
      opLine_dcache = cacheBSize>1?0:1;
      dcacheState = CSTATE_RD;
      return false;

    case CACHE_I:
      if((cacheBSize==1)&&(icacheState==CSTATE_RD))
        return false;
      if(icacheState && (line >= opLine_icache))
        return false;
      srcCache = icache[block];
      if(srcCache.valid && (srcCache.tag == tag))
        break;
      if(mPenalty_icache > 0)
        return false;
      if(MemBusy)
        return false;

      mPenalty_icache = MISS_PENALTY;
      opAddr_icache = addr;
      opLine_icache = cacheBSize>1?0:1;
      icacheState = CSTATE_RD;
      return false;
    default:
      Error("Error: Wrong Cache type");
  }
  *data = srcCache.block[line];
  return true;
}
*/

// TODO: need to change to base addr
bool writebackCache(cachedata* cacheData, unsigned int addr) {
  writebuffer* wbData;
  wbData = NULL;

  if(wrPolicy == POLICY_WB) {
    if(wrbuffer[WRBUFF_SIZE-2]) {
      return false;
    }

    // queue the data for WB
    wbData = createWRBuffer_WB(cacheData, addr, cacheBSize);
    if(!wbData) Error("Error: Unable to allocation memory");
    wrbuffer[WRBUFF_SIZE-2] = wbData;
    return true;
  }
  return true;
}

bool writeToCache(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize) {
  unsigned int cacheData;
  if(!CACHE_ENABLED) {
    return handleWRCDisabled(addr, data, offset, wsize);
  }
  // CACHE WRITE OPERATION

  if(wrbuffer[WRBUFF_SIZE-2])
    return false;

  switch(wrPolicy) {
    case POLICY_WB:
      if(!readDataCache(addr, &cacheData)) return false;

      data = getWrData(cacheData, data, offset, wsize);
      updateCache(addr, data);
      break;
    case POLICY_WT:
      if(!readDataCache(addr, &cacheData))
        return false;
      data = getWrData(cacheData, data, offset, wsize);
      wrbuffer[WRBUFF_SIZE-2] = createWRBuffer_WT(addr, data);
      updateCache(addr, data);
      break;
  }
  return true;
}

void updateCache(unsigned int addr, unsigned int data) {
  unsigned int block, line, tag;

  convertAddr(CACHE_D, &addr, &tag, &block, &line);
  (dcache[block]).dirty = true;
  (dcache[block]).block[line] = data;
}

bool handleWRCDisabled(unsigned int addr, unsigned int data, unsigned short offset, lwsw_len wsize) {
  memory[addr] = getWrData(memory[addr], data, offset, wsize);
  return true;
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
  mPenalty_icache = 0;
  opLine_icache = 0;
  opAddr_icache = 0;

  dcacheState = CSTATE_IDLE;
  mPenalty_dcache = 0;
  opLine_dcache = 0;
  opAddr_dcache = 0;

  if(cacheBSize == 1) {
    opLine_dcache = 1;
    opLine_icache = 1;
  }

  return;
}

void startCaching() {
  if(!CACHE_ENABLED) return;

  //cachePenalty = cachePenalty>0?cachePenalty-1:0;

  mPenalty_dcache = mPenalty_dcache>0?mPenalty_dcache-1:0;
  mPenalty_icache = mPenalty_icache>0?mPenalty_icache-1:0;


  if(!MemBusy && icacheState && !mPenalty_icache) {
    switch(icacheState) {
      case CSTATE_RD:
        if(cacheBSize == 1) {
          //opLine_icache++;
          icacheState = CSTATE_IDLE;
          fillCache(CACHE_I, opAddr_icache);
        }
        else {
          opLine_icache++;
          icacheState = CSTATE_RD_SUB;
          mPenalty_icache += SUBLINE_PENALTY;
          fillCache(CACHE_I, opAddr_icache++);
        }

        break;
      case CSTATE_RD_SUB:
        if(mPenalty_icache) break;

        opLine_icache++;
        fillCache(CACHE_I, opAddr_icache++);
        if(opLine_icache == cacheBSize) {
          icacheState = CSTATE_IDLE;
          break;
        }
        mPenalty_icache += SUBLINE_PENALTY;
        break;
      default:
        //Error("Unexpected Cache State");
        break;
    }
  }

  if(!MemBusy && dcacheState && !mPenalty_dcache) {
    switch(dcacheState) {
      case CSTATE_RD:
        if(cacheBSize == 1) {
          //opLine_dcache++;
          dcacheState = CSTATE_IDLE;
          fillCache(CACHE_D, opAddr_dcache);
        }
        else{
          opLine_dcache++;
          dcacheState = CSTATE_RD_SUB;
          mPenalty_dcache += SUBLINE_PENALTY;
          fillCache(CACHE_D, opAddr_dcache++);
        }

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
        //Error("Unexpected Cache State");
        break;
    }
  }
  return;
}
