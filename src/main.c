#include "units.h"
#include "pipeline.h"

unsigned int indexTest;
int test[5];

int main() {
  int tests[][5] = {
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
    {1, 64,  256, 1, POLICY_WT},
    {1, 64,  256, 1, POLICY_WB}
  };

  clock = 0;
  while(indexTest < 8) {
    printf("\n------\nRound %u\n------\n", indexTest);
    memcpy(&test, &tests[indexTest], sizeof(int)*5);
    init_utils();
    init_units();
    init_pipeline();

    while(1){
        start();
        clock++;
        if(PC==0) break;
    }
    printSummary();
    indexTest++;
    clock = 0;
  }
  printf("try to access cache[%d]", icache[0].valid);
	return 0;
}

/*

*/
