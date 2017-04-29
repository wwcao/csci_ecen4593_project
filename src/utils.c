#include "utils.h"

void init_utils() {
  numIns = 0;
  numBranch = 0;
  numLWSW = 0;
  numR_f = 0;
  numI_f = 0;
  numNop = 0;
  numStall = 0;
  numHarzard = 0;
  numRead_I = 0;
  numRead_D = 0;
  numReadMissed_I = 0;
  numReadMissed_D = 0;
  insCounter = 0;
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
    if(!(f = fopen(path, "r"))) {
      printf("Error: unable to read machine code from %s\n", path);
      exit(1);
    }

    addr = 0;
    while(fgets(buffer, MAX_READLINE, f)
            && buffer[0] != '\n') {
      if(buffer[0] != '0') continue;
      sscanf(buffer, "%x", &ins);
      if(addr >= MEMORY_SIZE) Error("Out of Bound\n");
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
    if(target[i]) {
        printf("Deleted a unempty write buffer\n");
        free(target[i]);
    }
  }

  free(target);
}

void statPipeline(count_type itype) {
  switch(itype) {
    case TYPE_R:
      numR_f++;
      break;
    case TYPE_I:
      numI_f++;
      break;
    case TYPE_LWSW:
      numLWSW++;
      break;
    case TYPE_NOP:
      numNop++;
      break;
    case TYPE_BR:
      numBranch++;
      break;
    case TYPE_STALL:
      numStall++;
      break;
    case TYPE_HARZARD:
      numHarzard++;
      break;
  }
}

void printSummaryHeader(const char* progName) {
  printf("\t>>[%s]<<\n", progName);
  printf("\ticache\t dcache\t       \t      \t Early \t\t      \t       \t        Total \t   Total  \t\n");
  printf("\t size \t  size \t block \t WT(0)\t       \t\ti-hit \t d-hit \t  CPI   clock \t          \tinclude\n");
  printf("\t(byte)\t (byte)\t(lines)\t WB(1)\t Cached\t\trate(%%)\trate(%%)\t       cycles\t  Inst. \t nop&stall\n");
}

void printSummary(const char* argName, const char** progNames, unsigned int len) {
  int prog1Min, prog2Min;
  int i, mark;
  stat_result *result;
  const char fheader[] = "ProgNum,icache size,dcache size,line,WT(0)/WB(1),Early Cached,icache Hit rate,dcache Hit rate,cpi,clock,total instructions\n";
  char buffer[256];
  char foutput[256];
  char filename[32];
  FILE* output;

  sprintf(filename, "results/%s%s", argName, ".csv");
  output = fopen(filename, "w");

  if(!output) Error("Error: Unable to open output file");
  mark = -1;
  result = results;
  fwrite(fheader, sizeof(char), strlen(fheader), output);
  for(i = 0; i < numTest; i++) {
    memset(buffer, 0, 256);
    memset(foutput, 0, 256);
    if(mark != result->progNum) {
      // Make this Simpler
      prog1Min = findMinCpi(0);
      prog2Min = findMinCpi(1);
      mark = result->progNum;
      printf("\n\n");
      printSummaryHeader(progNames[mark]);
    }


    if(i == prog1Min || i == prog2Min) {
      sprintf(buffer, "->%d", i);
    }
    else{
      sprintf(buffer, "  %d", i);
    }
    if(result->Cached) {
      sprintf(buffer, "%s\t%5d\t%5d\t%5d\t%3d\t%3d\t", buffer,
              result->icacheSize, result->dcacheSize, result->block_size, result->policy, result->PreCached);
      sprintf(buffer,"%s\t%3.2f\t%3.2f\t%3.3f\t%5d\t", buffer, result->iHitRate, result->dHitRate, result->cpi, result->clock);
      sprintf(foutput, "%d,%d,%d,%d,%d,%d,%.2f,%.2f,%.2f,%d,%d\n",
              result->progNum, result->icacheSize, result->dcacheSize, result->block_size, result->policy, result->PreCached,
              result->iHitRate, result->dHitRate,
              result->cpi, result->clock, result->ins);
    }
    else {
      //sprintf(buffer, "%s\t------\t------\t------\t------\t", buffer, result->icacheSize, result->dcacheSize, result->block_size, result->policy);
      sprintf(buffer, "%s\t------\t------\t------\t----\t----\t", buffer);
      sprintf(buffer, "%s\t%3.2f\t%3.2f\t%3.3f\t%5d\t", buffer, 0.0, 0.0, result->cpi, result->clock);
      sprintf(foutput, "%d,0,0,0, 0,0,0,0,%.2f,%d,%d\n",
              result->progNum, result->cpi, result->clock, result->ins);
    }

    sprintf(buffer,"%s  %d  \t%d\n", buffer, result->ins, result->insCounter);

    printf("%s", buffer);
    fwrite(foutput, sizeof(char), strlen(foutput), output);
    result++;
  }
  fclose(output);
}

void printConfig(const char* src, int* config, unsigned int len) {
  printf("\n");
  printf("[%s], ",  src);
  if(config[5]) {
    printf("ICache[%d], DCache[%d], Line[%d], ", config[1], config[2], config[3]);
    if(config[4]) {
      printf("WB, ");
    }
    else {
      printf("WT, ");
    }
    if(config[6]) {
      printf("Early Cached\n");
    }
    else {
      printf("Not Early Cached\n");
    }
  }
  else {
    printf("No cache\n");
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
    if(result->progNum == progNum &&
       min > result->cpi && result->Cached) {
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

  numI_f += numBranch + numLWSW + numNop;
  //numIns =  numI_f + numR_f;
  numIns -= numHarzard;
  cpi = (float)clock/numIns;

  hitRate_D = 100*(1-(float)(numReadMissed_D)/(numIns));
  hitRate_I = 100*(1-(float)(numReadMissed_I)/(numIns));

  memset(&result, 0, sizeof(stat_result));

  result.clock = clock;
  result.ins = numIns;
  result.insCounter = insCounter;
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
  result.stall = numStall;
  if(CacheEnabled) {
    result.iRd = numReadMissed_I;
    result.iRdMissed = numReadMissed_I;
    result.dRd = numReadMissed_D;
    result.dRdMissed = numReadMissed_D;
    result.dWr = numWrite_D;
    result.dWrMissed = numWriteMissed_D;

    result.iHitRate = hitRate_I;
    result.dHitRate = hitRate_D;
  }
  result.Cached = CacheEnabled;
  result.PreCached = PreCached;
  memcpy(&(results[index]), &result, sizeof(stat_result));
  return;
}

bool testResults(unsigned int index, int* config) {
  int j,k,l,m;
  switch(config[0]) {
    case 0:
      j = progResults[0][0]^memory[6];
      k = progResults[0][1]^memory[7];
      l = progResults[0][2]^memory[8];
      m = progResults[0][3]^memory[9];
      break;
    case 1:
      j = progResults[1][0]^memory[6];
      k = progResults[1][1]^memory[7];
      l = progResults[1][2]^memory[8];
      m = progResults[1][3]^memory[9];
      break;
    default:
      Error("Unknown program");
  }
  if(j||k||l||m) {
    printf("Config[%2d]...[FAILED]", index);
  }
  else {
    printf("Config[%2d]...[OK]", index);
  }
  printf(" memory[6-9][%08x][%08x][%08x][%08x]\n", memory[6],memory[7],memory[8],memory[9]);
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

bool readConfigs(const char* filename, unsigned int *confignum) {
  int i;
  unsigned int testNum, progNum;
  char buffer[MAX_READLINE];
  int* newConfig;
  FILE* fconf;

  fconf = fopen(filename,"r");
  if(!fconf) Error("Error: Unable to read config file");

  testNum = 0;
  progNum = 0;
  memset(buffer, 0, 64);
  while(fgets(buffer, MAX_READLINE, fconf)) {
    char c = buffer[0];
    if(c == '#') {
      progNum++;
      continue;
    }
    if( c < 48 || c > 57) continue;
    testNum++;
  }

  if(!(testNum>0)) Error("Error: Invalid configuration");

  rewind(fconf);
  fileTests = (int**)calloc(testNum, sizeof(int*));
  if(!fileTests) Error("Unable to allocation memory @ utils.c line 509");
  i = 0;
  while(fgets(buffer, MAX_READLINE, fconf)) {
    newConfig = NULL;
    char c = buffer[0];
    if(c < 48 || c > 57) continue;
    newConfig = (int*) calloc(7, sizeof(int));
    if(!newConfig) Error("Errror: Unable to allocate memory for configuration");
    sscanf(buffer, "%d,%d,%d,%d,%d,%d,%d", newConfig, newConfig+1, newConfig+2, newConfig+3,newConfig+4,
           newConfig+5, newConfig+6);
    fileTests[i] = newConfig;
    i++;
  }
  *confignum = testNum;
  return true;
}

void loadProgResults() {
  FILE* fresult;
  char buffer[MAX_READLINE];
  int* newRes;
  int counter;

  fresult = fopen("assembly_results.txt","r");
  if(!fresult) Error("Error: Unable to read Result file");


  progResults = (int**)calloc(2, sizeof(int*));
  counter = 0;
  while(fgets(buffer, MAX_READLINE, fresult)) {
    newRes = NULL;
    char c = buffer[0];
    if(c < 48 || c > 57) continue;
    newRes = (int*) calloc(4, sizeof(int));
    if(!newRes) Error("Errror: Unable to allocate memory for configuration");
    sscanf(buffer, "%x,%x,%x,%x", newRes, newRes+1, newRes+2, newRes+3);
    progResults[counter] = newRes;
    counter++;
  }

  counter = 0;
  for(counter = 0; counter < 2; counter++) {
    printf("%s---[0x%08x],[0x%08x],[0x%08x],[0x%08x]\n", progSources[counter],
           progResults[counter][0],progResults[counter][1], progResults[counter][2], progResults[counter][3]);
  }
  return;
}



