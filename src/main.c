#include "units.h"
#include "cache.h"
#include "pipeline.h"

unsigned int indexTest;
//int test[5];

const char *progSources[128] = {
  "Program1File.txt",
  "Program2File.txt",
  NULL
};

int main() {
  unsigned int testNum;
  int mark;
  int tests[][5] = {
    {0, 128, 256,   16, POLICY_WT},
    {0, 128, 256,   16, POLICY_WB},
    {0, 128, 256,   4,  POLICY_WT},
    {0, 128, 256,   4,  POLICY_WB},
    {0, 128, 256,   1,  POLICY_WT},
    {0, 128, 256,   1,  POLICY_WB},
    {0, 64,  1024,  16, POLICY_WT},
    {0, 64,  1024,  16, POLICY_WB},
    {0, 64,  1024,  4,  POLICY_WT},
    {0, 64,  1024,  4,  POLICY_WB},
    {0, 64,  1024,  1,  POLICY_WT},
    {0, 64,  1024,  1,  POLICY_WB},
    {1, 64,  512,   16, POLICY_WT},
    {1, 64,  512,   16, POLICY_WB},
    {1, 64,  512,   4,  POLICY_WT},
    {1, 64,  512,   4,  POLICY_WB},
    {1, 64,  512,   1,  POLICY_WT},
    {1, 64,  512,   1,  POLICY_WB},
    {1, 128, 256,   16, POLICY_WT},
    {1, 128, 256,   16, POLICY_WB},
    {1, 128, 256,   4,  POLICY_WT},
    {1, 128, 256,   4,  POLICY_WB},
    {1, 128, 256,   1,  POLICY_WT},
    {1, 128, 256,   1,  POLICY_WB},
    {1, 256, 128,   16, POLICY_WT},
    {1, 256, 128,   16, POLICY_WB},
    {1, 256, 128,   4,  POLICY_WT},
    {1, 256, 128,   4,  POLICY_WB},
    {1, 256, 128,   1,  POLICY_WT},
    {1, 256, 128,   1,  POLICY_WB}
  };
//  filepaths = filenames;
  testNum = sizeof(tests)/(sizeof(unsigned int)*5);
  mark = -1;
  while(indexTest < testNum) {

    memcpy(&config, &tests[indexTest], sizeof(int)*5);
    if(mark != config[0]) {
      if(!mark) printf("\n\n");
      mark = config[0];
      printf("program [%s]\n", progSources[mark]);
      printf("icache\t dcache\t       \t       \t      \t       \t        Total \n");
      printf(" size \t  size \t block \t WT(0) \ti-hit \t d-hit \t  CPI   clock \n");
      printf("(byte)\t (byte)\t(lines)\t WB(1) \trate(%%)\trate(%%)\t       cycles\n");

    }
    //printf("\n------\nTest %u \t%d\t%d\t%d\n------\n", indexTest, config[1], config[2], config[3]);
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
    //printSummary();
    printTestTable(config, 5);
    destroyCaches();
    destroyWRBuffer();
    indexTest++;
  }
	return 0;
}
