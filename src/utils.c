#include "utils.h"

void init_utils() {
  numIns = 0;
  numBranch = 0;
  numLWSW = 0;
  numR_I = 0;
  numNop = 0;
  numRead = 0;
  numWrite = 0;
  numReadMissed = 0;
  numWriteMissed = 0;
}

unsigned int readInstruction(const char* path) {
    int addr;
    unsigned int ins;
    char buffer[MAX_READLINE];

    printf("Filename: %s", path);
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
  unsigned int memSize;
  int i;

  newCache = NULL;
  memSize = sizeof(cache)*blockNum;
  newCache = (cache*)calloc(blockNum, sizeof(cache));
  if(!newCache)  {
    Error("Error: Unable to allocate memory");
  }

  for(i = 0; i < blockNum; i++) {
    newCData = NULL;
    newCData = (cachedata*)calloc(lineNum, sizeof(cachedata));
    if(!newCache) {
      destroyUnusedCache(newCache, blockNum);
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
}

void printPipelineStat() {
  float cpi;
  numIns = numBranch + numLWSW + numR_I;
  printf("\nPipeline\n");
  printf("_____________________________________\n");
  cpi = (float)clock/numIns;
  printf("Total intruction [%u]\n", numIns);
  printf("CPI: [%2.3f]\n", cpi);
  printf("Total Branch [%u]\n", numBranch);
  printf("Total LWSW [%u]\n", numLWSW);
  printf("Total R/I [%u]\n", numR_I);
  printf("Total NOP [%u]\n", numNop);

}

void printReadCacheStat() {
  printf("\nRead Cache Stat\n");
  printf("_____________________________________\n");
}

void printWriteCacheStat() {
  printf("\nWrite Cache Stat\n");
  printf("_____________________________________\n");
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

