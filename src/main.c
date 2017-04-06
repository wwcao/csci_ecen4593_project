#include <stdio.h>
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
	printf("clock[%d]\n", clock);
	printSummary();
	return 0;
}
