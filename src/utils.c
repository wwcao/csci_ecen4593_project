#include "utils.h"

void init_utils() {
  numIns = 0;
  numBranch = 0;
  numLWSW = 0;
  numR_f = 0;
  numI_f = 0;
  numNop = 0;
  numRead_I = 0;
  numRead_D = 0;
  numReadMissed_I = 0;
  numReadMissed_D = 0;

  numWrite_D = 0;
  numWriteMissed_D = 0;
}

unsigned int getWrData(unsigned int cacheData, unsigned int newData, unsigned short offset, lwsw_len wsize) {
  unsigned int shamt;
  unsigned int mask;
  unsigned int res;
  switch(wsize) {
    case DLEN_W:
      res = newData;
      break;
    case DLEN_B:
      shamt = (3-offset)*8;
      newData = ((newData)&0xff)<<shamt;
      mask = 0xff<<shamt;
      res = (cacheData&(~mask))|newData;
      break;
    case DLEN_HW:
      shamt = (1-offset)*16;
      newData = ((newData)&0xffff)<<shamt;
      mask = 0xffff<<shamt;
      res = (cacheData&(~mask))|newData;
      break;
    default:
      Error("Unexpected data length");
  }
  return res;
}


unsigned int readInstruction(const char* path) {
    int addr;
    unsigned int ins;
    char buffer[MAX_READLINE];

    FILE* f;
    if(!(f = fopen(path, "r"))) Error("Failed to read file\n");

    addr = 0;
    while(fgets(buffer, MAX_READLINE, f)
            && buffer[0] != '\n') {
      if(buffer[0] != '0') continue;
      sscanf(buffer, "%x", &ins);
      if(addr > MEMORY_SIZE) Error("Out of Bound\n");
        memory[addr] = ins;
      addr++;
    }
    fclose(f);
    return addr;
}

op_format getInsFormat(int ins) {
    if((ins&OP_MASK) == 2 || (ins&OP_MASK) == 3){
      // 0x02 0x03
      printf("0x%x is J Format\n",ins);
      return FORMAT_J;
    }else if(ins&OP_MASK){
        // non-zero I Type
        printf("0x%x is I Format\n", ins);
        return FORMAT_I;
    }
    printf("0x%x is R Format\n", ins);
    return FORMAT_R;
}

unsigned getPartNum(int ins, part_type ptype) {
	switch(ptype) {
		case PART_OP:
			return (ins&OP_MASK)>>OP_SHF;
		case PART_RS:
			return (ins&RS_MASK)>>RS_SHF;
		case PART_RT:
			return (ins&RT_MASK)>>RT_SHF;
		case PART_RD:
			return (ins&RD_MASK)>>RD_SHF;
		case PART_SHM:
			return (ins&SH_MASK)>>SHM_SHF;
		case PART_FUNC:
			return (ins&FN_MASK);
		case PART_IMM:
			return (ins&IM_MASK);
		default:
			Error("Instruction: Wrong format\n");
			exit(1);
	}
}

cache* createCache(unsigned int blockNum, unsigned int lineNum) {
  cache* newCache;
  cachedata* newCData;
  //unsigned int memSize;
  int i;

  newCache = NULL;
//  memSize = sizeof(cache)*blockNum;
  newCache = (cache*)calloc(blockNum, sizeof(cache));
  if(!newCache)  {
    Error("Error: Unable to allocate memory");
  }

  for(i = 0; i < blockNum; i++) {
    newCData = NULL;
    newCData = (cachedata*)calloc(lineNum, sizeof(unsigned int));
    if(!newCache) {
      Error("Error: Unable to allocate memory");
    }
    (newCache[i]).block = newCData;
  }
  return newCache;
}

void destroyUnusedCache(cache* target, unsigned int blockNum) {
  int i;
  void* data;

  if(!target) return;

  for(i = 0; i < blockNum; i++) {
    if((data = target[i].block)) {
      free(data);
    }
  }
  free(target);
}

writebuffer** initWRBuffers(writebuffer** des) {
  writebuffer **newWRBuffer;

  newWRBuffer = (writebuffer**)calloc(WRBUFF_SIZE, sizeof(int*));
  if(!newWRBuffer) Error("Error: Unable to allocate memory for *WRBuffer");

  des = newWRBuffer;
  return newWRBuffer;
}

writebuffer* createWRBuffer_WT(unsigned int addr, unsigned int data) {
  writebuffer* newWRBuffer;

  newWRBuffer = (writebuffer*)calloc(1, sizeof(writebuffer));

  if(!newWRBuffer) {
    Error("Error: Unable to allocate memory for WRBuffer");
  }

  newWRBuffer->addr = addr;
  newWRBuffer->data = data;

  return newWRBuffer;
}

writebuffer* createWRBuffer_WB(cachedata* cacheData, unsigned addr, int lineNum) {
  int i;
  writebuffer* newWRBuffer;

  newWRBuffer = (writebuffer*)calloc(lineNum, sizeof(writebuffer));

  if(!newWRBuffer) {
    Error("Error: Unable to allocate memory for WRBuffer");
  }

  for(i = 0; i < lineNum; i++) {
    newWRBuffer[i].addr = addr;
    newWRBuffer[i].data = cacheData[i];
    addr++;
  }

  return newWRBuffer;
}

void destroyUnusedWRBuffer(writebuffer** target) {
  int i;
  if(!target) return;
  for(i=0; i < WRBUFF_SIZE; i++) {
    if(target[i]) free(target[i]);
  }

  free(target);
}

void statPipeline(ins_type itype) {
  switch(itype) {
    case ALUOP_R:
      numR_f++;
      break;
    case ALUOP_I:
      numI_f++;
      break;
    case ALUOP_LWSW:
      numLWSW++;
      break;
    case ALUOP_NOP:
      numNop++;
      break;
    case ALUOP_BR:
      numBranch++;
      break;
  }
}


void printSummary() {
  printf("result: \n[0x%08x]\n[0x%08x]\n[0x%08x]\n[0x%08x]\n",
		memory[6],memory[7],memory[8],memory[9]);
  printf("=====================================\n");
  printf("  Summary\n");
  printf("Cycle[%u]\n", clock);
  printf("=====================================\n");
  printPipelineStat();
  printReadCacheStat();
  printWriteCacheStat();
  printCacheStat();
}

void printPipelineStat() {
  float cpi;
  numI_f += numBranch + numLWSW;
  numIns =  numI_f + numR_f;
  printf("\nPipeline\n");
  printf("_____________________________________\n");
  cpi = (float)clock/numIns;
  printf("Total instruction [%u]\n", numIns);
  printf("CPI: [%2.3f]\n", cpi);
  printf("Total Branch [%u]\n", numBranch);
  printf("Total LWSW [%u]\n", numLWSW);
  printf("Total R/I [%u]\n", numR_f);
  printf("Total NOP [%u]\n", numNop);

}

void printReadCacheStat() {
  printf("\nRead Cache Stat\n");
  printf("_____________________________________\n");
  printf("Total I-Read [%u]\n",numRead_I);
  printf("Total I-Read Missed [%u]\n",numReadMissed_I);

  printf("Total D-Read [%u]\n",numRead_D);
  printf("Total D-Read Missed [%u]\n",numReadMissed_D);

}

void printWriteCacheStat() {
  printf("\nWrite Cache Stat\n");
  printf("_____________________________________\n");
  printf("Total D-Write [%u]\n",numWrite_D);
  printf("Total D-Write Missed [%u]\n",numWriteMissed_D);
}

void printCacheStat(){
  float hitRate_D;
  float hitRate_I;
  hitRate_D = 100*(1-(float)(numReadMissed_D+numWriteMissed_D)/(numRead_D+numWrite_D));
  hitRate_I = 100*(1-(float)(numReadMissed_I)/numRead_I);

  printf("\nCache Stat\n");
  printf("_____________________________________\n");
  printf("I-Hit Rate [%2.2f]%%\n",hitRate_D);
  printf("D-Hit Rate [%2.2f]%%\n",hitRate_I);

}

void Error(const char* msg) {
    printf("Error: %s\n", msg);
    exit(1);
}

void Log(const char* format, ...) {
  // log
  /*
  va_list argptr = NULL;
  va_start(argptr, format);
  vfprintf(format, argptr);
  va_end(argptr);
  */
  return;
}

void printRegisters() {
	int i;
	printf("PC[0x%04d]\n", PC);
	for( i = 0; i < REGISTER_SIZE; i+=4) {
		printf("$%02d[0x%08x], ", i	, register_file[i]);
		printf("$%02d[0x%08x], ", i+1, register_file[i+1]);
		printf("$%02d[0x%08x], ", i+2, register_file[i+2]);
		printf("$%02d[0x%08x]\n", i+3, register_file[i+3]);
	}
}

void testSum(unsigned int total) {
  unsigned int i;
  unsigned int start = 243;
  int sum;

  sum = 0;
  for(i = start; i < total+start; i++) {
    sum += memory[i];
  }
  printf("Sum: [%d][%08x]\n", sum, sum);
}

bool findLBits(unsigned int num, unsigned short *bNum, unsigned int *mask) {
  switch(num) {
    case 1:
      *bNum = 0;
      *mask = 0x0;
      return true;
    case 2:
      *bNum = 1;
      *mask = 0x1;
      return true;
    case 4:
      *bNum = 2;
      *mask = 0x3;
      return true;
    case 8:
      *bNum = 3;
      *mask = 0x7;
      return true;
    case 16:
      *bNum = 4;
      *mask = 0xf;
      return true;
    case 32:
      *bNum = 5;
      *mask = 0x1f;
      return true;
    default:
      Error("findLBits");
  }
  return false;
}
