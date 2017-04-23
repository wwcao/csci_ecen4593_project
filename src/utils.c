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

void init_results(unsigned int num) {
  numTest = num;
  results = (stat_result*)calloc(numTest, sizeof(stat_result));
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

void printSummaryHeader(const char* progName) {
  printf("program [%s]\n", progName);
  printf("icache\t dcache\t       \t       \t      \t       \t        Total \t  Total   \n");
  printf(" size \t  size \t block \t WT(0) \ti-hit \t d-hit \t  CPI   clock \t          \n");
  printf("(byte)\t (byte)\t(lines)\t WB(1) \trate(%%)\trate(%%)\t       cycles\tInstruction\n");
}

void printSummary(const char** progNames, unsigned int len) {
  int prog1Min, prog2Min;
  int i, mark;
  stat_result *result;

  // Make this Simpler
  prog1Min = findMinCpi(0);
  prog2Min = findMinCpi(1);

  mark = -1;
  result = results;
  for(i = 0; i < numTest; i++) {
    if(mark != result->progNum) {
      mark = result->progNum;
      printf("\n\n");
      printSummaryHeader(progNames[mark]);
    }


    if(i == prog1Min || i == prog2Min) {
      printf("Min ------------------------------------------------------------\n");
    }
    printf("%5d\t%5d\t%5d\t%5d\t", result->icacheSize, result->dcacheSize, result->block_size, result->policy);
    printf("%0.2f\t%0.2f\t%1.3f\t%d\t\n", result->iHitRate, result->dHitRate, result->cpi, result->clock);
    if(i == prog1Min || i == prog2Min) {
      printf("----------------------------------------------------------------\n");
    }

    result++;
  }
}

unsigned int findMinCpi(unsigned int progNum) {
  unsigned int i;
  unsigned int res;
  float min;
  stat_result *result;

  res = 0;
  result = results;
  min = 1000;
  for(i = 0; i < numTest; i++) {
    if(result->progNum == progNum && min > result->cpi) {
      min = result->cpi;
      res = i;
    }
    result++;
  }
  return res;
}

void saveResult(int index, int* config) {
  stat_result result;
  float hitRate_D;
  float hitRate_I;

  hitRate_D = 100*(1-(float)(numReadMissed_D+numWriteMissed_D)/(numRead_D+numWrite_D));
  hitRate_I = 100*(1-(float)(numReadMissed_I)/numRead_I);

  numI_f += numBranch + numLWSW;
  numIns =  numI_f + numR_f;
  cpi = (float)clock/numIns;

  result.clock = clock;
  result.ins = numIns;
  result.cpi = cpi;

  result.progNum = config[0];
  result.icacheSize = config[1];
  result.dcacheSize = config[2];
  result.block_size = config[3];
  result.policy = config[4];

  result.iformat = numBranch + numLWSW;
  result.rformat = numR_f;
  result.br = numBranch;
  result.lwsw = numLWSW;
  result.nop = numNop;

  result.iRd = numReadMissed_I;
  result.iRdMissed = numReadMissed_I;
  result.dRd = numReadMissed_D;
  result.dRdMissed = numReadMissed_D;
  result.dWr = numWrite_D;
  result.dWrMissed = numWriteMissed_D;

  result.iHitRate = hitRate_I;
  result.dHitRate = hitRate_D;
  memcpy(&(results[index]), &result, sizeof(stat_result));
  return;
}

bool testResults(unsigned int index, int* config) {
  int j,k,l,m;
  switch(config[0]) {
    case 0:
      j = memory[6]^112;
      k = memory[7]^29355;
      l = memory[8]^14305;
      m = memory[9]^0;
      if(j||k||l||m) {
        printf("Config[%2d] has different result\n", index);
        return false;
      }
      break;
    case 1:
      j = memory[7]^0x20696e71;
      k = memory[8]^0x206e7376;
      l = memory[9]^0x20696e71;
      m = memory[6]^1;
      if(j||k||l||m) {
        printf("Config[%2d] has different result\n", index);
        return false;
      }
      break;
    default:
      Error("Unknown program");
  }
  printf("Config[%2d]....[OK], memory[6-9][%08x][%08x][%08x][%08x]\n", index, memory[6],memory[7],memory[8],memory[9]);
  return true;
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
