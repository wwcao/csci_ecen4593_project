#include "units.h"
#include "cache.h"
#include "pipeline.h"

int** fileTests;
int** progResults;
bool Check;

const char *progSources[128] = {
  "Program1File.txt",
  "Program2File.txt"
};


void testMemory() {
  int i;
  for(i = 0; i < 70; i++) {
    printf("[0x%x][0x%x], %d\n", memory[200+i], memory[470+i], memory[200+i]^memory[470+i]);
  }
}

int main(int argc, char** argv) {
  int start, end;
  unsigned int testNum, indexTest;

	Check = true;
	indexTest = 0;
	testNum = 0;

  if(argc < 2) {
    Error("Need a config file");
  }
	if(argc > 2 && !strcmp(argv[2], "-s")) {
		printf("Check: Disabled\n");
		Check = false;
	}

  readConfigs(argv[1], &testNum);
  loadProgResults();

  if(UNIFIEDCACHE) {
    printf("Unified Cache, D-Cache size is used for both Instruction and Data\n");
  }

  init_results(testNum);
  start = 0;
  end = 0;
  while(indexTest < testNum) {
    int* curTest = *(fileTests+indexTest);
    memcpy(&config, curTest, sizeof(int)*7);
		if(Check) printConfig(progSources[config[0]], config, 7);
    init_utils();
    init_units();
    init_pipeline();
    while(1){
      updateMemory();
      startCaching();
      startPipeline();
      if(PC==0&&(!checkMemory()))
        break;
      clock++;
    }
		if(Check) testResults(indexTest, config);

    saveResult(indexTest, config);
    destroyCaches();
    destroyWRBuffer();
    indexTest++;
    free(curTest);
  }
  printSummary(argv[1], progSources, 2);
  printf(">%d<\n", start);
  testMemory();
	if(results) free(results);
  if(fileTests) free(fileTests);

  free(progResults[0]);
  free(progResults[1]);
  free(progResults);
	return 0;
}




