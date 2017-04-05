#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

extern unsigned int clock;

int main() {
	loadInstructions();
	init_units();
	init_pipeline();
	clock = 0;
	while(1){
			start();
			clock++;
			if(PC==0) break;
			if(PC==160)
        printf("asdf");
	}
	printf("clock[%d]\n", clock++);
	printf("result: \n[0x%08x]\n[0x%08x]\n[0x%08x]\n[0x%08x]\n",
		memory[6],memory[7],memory[8],memory[9]);
  printSummary();
	return 0;
}
