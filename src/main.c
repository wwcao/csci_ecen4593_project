#include "units.h"
#include "pipeline.h"

unsigned int indexTest;
int test[5];

const char *progSources[128] = {
  "program1File.txt",
  "program2File.txt",
  NULL
};

int main() {
  unsigned int testNum;
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
    //
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
  while(indexTest < testNum) {
    clock = 0;
    printf("\n------\nRound %u\n------\n", indexTest);
    memcpy(&test, &tests[indexTest], sizeof(int)*5);

    init_utils();
    init_units();
    init_pipeline();
    while(1){
        startPipeline();
        clock++;
        if(PC==0) break;
    }

    printSummary();
    destroyCaches();
    indexTest++;
  }
  printf("try to access cache[%d]", icache[0].valid);
	return 0;
}

/*

*/
