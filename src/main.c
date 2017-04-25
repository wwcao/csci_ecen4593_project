#include "units.h"
#include "cache.h"
#include "pipeline.h"

unsigned int indexTest;
//int test[5];

int** fileTests;

const char *progSources[128] = {
  "Program1File.txt",
  "Program2File.txt"
};

int main(int argc, char** argv) {
  unsigned int testNum;
  if(argc < 2) {
    Error("Need a config file");
  }
  else {
    readConfigs(argv[1], &testNum);
  }

  if(UNIFIEDCACHE) {
    printf("Unified Cache, D-Cache size is used for both Instruction and Data\n");
  }
  init_results(testNum);
  while(indexTest < testNum) {
    int* curTest = *(fileTests+indexTest);
    memcpy(&config, curTest, sizeof(int)*7);
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
    testResults(indexTest, config);
    saveResult(indexTest, config);
    destroyCaches();
    destroyWRBuffer();
    indexTest++;
    free(curTest);
  }

  printSummary(argv[1], progSources, 2);

  if(results) free(results);
  if(fileTests) free(fileTests);
	return 0;
}
