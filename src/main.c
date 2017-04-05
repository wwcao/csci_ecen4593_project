#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

extern unsigned int clock;

int main() {
  clock = 0;
	loadInstructions();
	init_units();
	init_pipeline();
	init_utils();
	while(1){
			start();
			clock++;
			if(PC==0) break;
	}
	printf("clock[%d]\n", clock++);
	printf("result: \n[0x%08x]\n[0x%08x]\n[0x%08x]\n[0x%08x]\n",
		memory[6],memory[7],memory[8],memory[9]);
  printSummary();
	return 0;
}
