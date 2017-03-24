#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

unsigned int memory[MEMORY_SIZE];
unsigned int register_file[REGISTER_NUM];
unsigned int PC;

int main() {
	unsigned int i;
	unsigned int counter;
	PC = 0;
	counter = 0;
	i = readInstruction();
	init_pipeline();
	while(1){
			start();
			if(((PC+4)>>2) > i) {
				printf("Done\n");
				break;
			}
			counter++;
			if(counter > 1000) break;
	}
	printf("Counter[%d]\n", counter++);
	return 0;
}
