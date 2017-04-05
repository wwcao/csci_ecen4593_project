#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

unsigned int counter;

int main() {
	loadInstructions();
	init_units();
	init_pipeline();
	while(1){
			start();
			counter++;
			if(counter == 990) {
        printf("counter is too larger\n");
			}
			if(PC == 70)
        printf("stop");
			if(!run_pipeline) break;
	}
	printf("Counter[%d]\n", counter++);
	printf("result: \n[0x%08x]\n[0x%08x]\n[0x%08x]\n[0x%08x]\n",
		memory[6],memory[7],memory[8],memory[9]);
	return 0;
}
