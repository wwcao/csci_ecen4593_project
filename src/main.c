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
	unsigned int counter;
	PC = 0;
	counter = 0;
	i = readInstruction();
	while(1){
			start();
			//printRegisters();
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
