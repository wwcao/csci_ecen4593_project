#include <stdio.h>
#include "pipeline.h"


unsigned int clock;
unsigned int testNum;
int tests[8][5];

int main() {
  clock = 0;
  testNum = 0;

	//loadInstructions();
  while(testNum < 8) {
    init_utils();
    init_units();
    init_pipeline();
    while(1){
        start();
        clock++;
        if(PC==0) break;
    }
    printSummary();
    testNum++;
    clock = 0;
  }
  printf("DONE\n");
	return 0;
}

/*
{
  {0, 128, 256, 4, POLICY_WT},
  {0, 128, 256, 4, POLICY_WB},
  {0, 128, 256, 1, POLICY_WT},
  {0, 128, 256, 1, POLICY_WB},
  {0, 64,  256, 4, POLICY_WT},
  {0, 64,  256, 4, POLICY_WB},
  {0, 64,  256, 1, POLICY_WT},
  {0, 64,  256, 1, POLICY_WB},
  //
  {1, 128, 256, 4, POLICY_WT},
  {1, 128, 256, 4, POLICY_WB},
  {1, 128, 256, 1, POLICY_WT},
  {1, 128, 256, 1, POLICY_WB},
  {1, 64,  256, 4, POLICY_WT},
  {1, 64,  256, 4, POLICY_WB},
  {1, 64,  256, 1, POLICY_WT}
};
*/
