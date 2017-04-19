#include "memory.h"

void updateMemory() {
  unsigned int addr;
  unsigned int data;

  if(memoryPenalty > 0) {
    memoryPenalty--;
    if(memoryPenalty > 0)
      return;
    switch(wrPolicy) {
      case POLICY_WB:

        addr = wrbuffer[WRBUFF_SIZE-1][opLine_mem].addr;
        data = wrbuffer[WRBUFF_SIZE-1][opLine_mem].data;
        memory[addr] = data;
        opLine_mem++;
        if(opLine_mem == cacheBSize) {
          MemBusy = false;
          free(wrbuffer[WRBUFF_SIZE-1]);
          wrbuffer[WRBUFF_SIZE-1] = NULL;
          break;
        }
        memoryPenalty = SUBLINE_PENALTY;
        break;
      case POLICY_WT:
        addr = wrbuffer[WRBUFF_SIZE-1]->addr;
        data = wrbuffer[WRBUFF_SIZE-1]->data;
        memory[addr] = data;
        MemBusy = false;
        free(wrbuffer[WRBUFF_SIZE-1]);
        wrbuffer[WRBUFF_SIZE-1] = NULL;
        break;
    }
  }

  if(!memoryPenalty && wrbuffer[WRBUFF_SIZE-2]) {
    setMemWrite();
  }

  return;
}


bool setMemWrite() {

  if(MemBusy||wrbuffer[WRBUFF_SIZE-1])
    return false;
  //if(icacheState||dcacheState) return false;
  wrbuffer[WRBUFF_SIZE-1] = wrbuffer[WRBUFF_SIZE-2];
  wrbuffer[WRBUFF_SIZE-2] = NULL;
  opLine_mem = 0;
  MemBusy = true;
  memoryPenalty = MISS_PENALTY;
  return true;
}

void init_memoryCtl() {
  opAddr_mem = 0;
  opLine_mem = 0;
  memoryPenalty = 0;
}
