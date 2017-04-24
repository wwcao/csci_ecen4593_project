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
  int tests[][7] = {
  //prog, icache, dcache, line, WT/WB, Cache?, PreCache
    //
    // NO CACHE
    //
    {0, 128, 256,   16, POLICY_WT, 0, 0},
    {1, 64,  512,   16, POLICY_WT, 0, 0},
    //
    // Normal Tests
    //
    {0, 128, 256,   16, POLICY_WT, 1, 1},
    {0, 128, 256,   16, POLICY_WB, 1, 1},
    {0, 128, 256,   4,  POLICY_WT, 1, 1},
    {0, 128, 256,   4,  POLICY_WB, 1, 1},
    {0, 128, 256,   1,  POLICY_WT, 1, 1},
    {0, 128, 256,   1,  POLICY_WB, 1, 1},
    {0, 64,  1024,  16, POLICY_WT, 1, 1},
    {0, 64,  1024,  16, POLICY_WB, 1, 1},
    {0, 64,  1024,  4,  POLICY_WT, 1, 1},
    {0, 64,  1024,  4,  POLICY_WB, 1, 1},
    {0, 64,  1024,  1,  POLICY_WT, 1, 1},
    {0, 64,  1024,  1,  POLICY_WB, 1, 1},
    {1, 64,  512,   16, POLICY_WT, 1, 1},
    {1, 64,  512,   16, POLICY_WB, 1, 1},
    {1, 64,  512,   4,  POLICY_WT, 1, 1},
    {1, 64,  512,   4,  POLICY_WB, 1, 1},
    {1, 64,  512,   1,  POLICY_WT, 1, 1},
    {1, 64,  512,   1,  POLICY_WB, 1, 1},
    {1, 128, 256,   16, POLICY_WT, 1, 1},
    {1, 128, 256,   16, POLICY_WB, 1, 1},
    {1, 128, 256,   4,  POLICY_WT, 1, 1},
    {1, 128, 256,   4,  POLICY_WB, 1, 1},
    {1, 128, 256,   1,  POLICY_WT, 1, 1},
    {1, 128, 256,   1,  POLICY_WB, 1, 1},
    {1, 256, 128,   16, POLICY_WT, 1, 1},
    {1, 256, 128,   16, POLICY_WB, 1, 1},
    {1, 256, 128,   4,  POLICY_WT, 1, 1},
    {1, 256, 128,   4,  POLICY_WB, 1, 1},
    {1, 256, 128,   1,  POLICY_WT, 1, 1},
    {1, 256, 128,   1,  POLICY_WB, 1, 1},

    //
    // min?
    //
    {0, 128, 1024,   16,  POLICY_WB, 1, 1},
    {0, 128, 1024,   8,  POLICY_WB, 1, 1},
    {0, 128, 1024,   4,  POLICY_WB, 1, 1},
    {0, 128, 1024,   2,  POLICY_WB, 1, 1},
    {0, 128, 1024,   1,  POLICY_WB, 1, 1},
    {0, 128, 1024,   16,  POLICY_WT, 1, 1},
    {0, 128, 1024,   8,  POLICY_WT, 1, 1},
    {0, 128, 1024,   4,  POLICY_WT, 1, 1},
    {0, 128, 1024,   2,  POLICY_WT, 1, 1},
    {0, 128, 1024,   1,  POLICY_WT, 1, 1},
    //
    // min ?
    //
    {1, 128, 1024,   16,  POLICY_WB, 1, 1},
    {1, 128, 1024,   8,  POLICY_WB, 1, 1},
    {1, 128, 1024,   4,  POLICY_WB, 1, 1},
    {1, 128, 1024,   2,  POLICY_WB, 1, 1},
    {1, 128, 1024,   1,  POLICY_WB, 1, 1},
    {1, 128, 1024,   16,  POLICY_WT, 1, 1},
    {1, 128, 1024,   8,  POLICY_WT, 1, 1},
    {1, 128, 1024,   4,  POLICY_WT, 1, 1},
    {1, 128, 1024,   2,  POLICY_WT, 1, 1},
    {1, 128, 1024,   1,  POLICY_WT, 1, 1},

    //
    // min? No precache
    //
    {0, 128, 1024,   16,  POLICY_WB, 1, 0},
    {0, 128, 1024,   8,  POLICY_WB, 1, 0},
    {0, 128, 1024,   4,  POLICY_WB, 1, 0},
    {0, 128, 1024,   2,  POLICY_WB, 1, 0},
    {0, 128, 1024,   1,  POLICY_WB, 1, 0},
    {0, 128, 1024,   16,  POLICY_WT, 1, 0},
    {0, 128, 1024,   8,  POLICY_WT, 1, 0},
    {0, 128, 1024,   4,  POLICY_WT, 1, 0},
    {0, 128, 1024,   2,  POLICY_WT, 1, 0},
    {0, 128, 1024,   1,  POLICY_WT, 1, 0},
    //
    // min ? No precache
    //
    {1, 128, 1024,   16,  POLICY_WB, 1, 0},
    {1, 128, 1024,   8,  POLICY_WB, 1, 0},
    {1, 128, 1024,   4,  POLICY_WB, 1, 0},
    {1, 128, 1024,   2,  POLICY_WB, 1, 0},
    {1, 128, 1024,   1,  POLICY_WB, 1, 0},
    {1, 128, 1024,   16,  POLICY_WT, 1, 0},
    {1, 128, 1024,   8,  POLICY_WT, 1, 0},
    {1, 128, 1024,   4,  POLICY_WT, 1, 0},
    {1, 128, 1024,   2,  POLICY_WT, 1, 0},
    {1, 128, 1024,   1,  POLICY_WT, 1, 0}
  };
//  filepaths = filenames;
  testNum = sizeof(tests)/(sizeof(unsigned int)*7);

  init_results(testNum);
  while(indexTest < testNum) {
    memcpy(&config, &tests[indexTest], sizeof(int)*7);
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
  }

  printSummary(progSources, 2);
  if(results) free(results);
	return 0;
}
