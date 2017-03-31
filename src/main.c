#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

int main() {
	unsigned int counter;
	loadInstructions();
	init_units();
	init_pipeline();
	while(1){
			start();
			counter++;
			if(PC == 0||PC==30) break;
			if(memory[74+721+2] > 0)
        continue;
	}
	printf("Counter[%d]\n", counter++);
	printf("result: \n[0x%08x]\n[0x%08x]\n[0x%08x]\n[0x%08x]\n",
		memory[6],memory[7],memory[8],memory[9]);
	return 0;
}
