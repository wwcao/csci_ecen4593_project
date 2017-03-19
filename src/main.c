#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

unsigned int memory[MEMORY_SIZE];
unsigned int register_file[REGISTER_NUM];
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
unsigned int PC;
int main() {
	unsigned int i;
	PC = 0;
	i = readInstruction();
	memory[100] = 4593;
	while(1){
			/*
			printf("type: %d\n", getInsFormat(memory[pc]));
			printf("rs: %d\n", getRegNum(memory[pc], REG_RS));
			printf("rd: %d\n", getRegNum(memory[pc], REG_RD));
			printf("rt: %d\n", getRegNum(memory[pc], REG_RT));
			* */
			start();
			printf("$t0:[%d], $s0:[%d]\n", register_file[8], register_file[16]);
			//pc++;
			if((PC>>2) == i) {
				printf("Done\n");
				break;
			}
	}
	printf("memory[100] = %d, should be 4000\n", memory[100]);
	return 0;
}
