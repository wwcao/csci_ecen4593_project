#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

unsigned int memory[MEMORY_SIZE];
/* used pc instead
unsigned int instruction;

int main() {
    readInstruction();
    instruction = 0;
    while(instruction < 10){
	

	start();
        instruction+=1;
    }

    return 0;
*/
unsigned int pc;
int main() {
	unsigned int i;
	pc = 0;
	i = readInstruction();
	while(1){
			/*
			printf("type: %d\n", getInsFormat(memory[pc]));
			printf("rs: %d\n", getRegNum(memory[pc], REG_RS));
			printf("rd: %d\n", getRegNum(memory[pc], REG_RD));
			printf("rt: %d\n", getRegNum(memory[pc], REG_RT));
			* */
			start();
			//pc++;
			if(pc > i) {
				printf("Done\n");
				break;
			}
	}

	return 0;
}
