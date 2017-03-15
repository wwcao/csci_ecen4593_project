#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"
#include "pipeline.h"

unsigned int memory[MEMORY_SIZE];
unsigned int instruction;

int main() {
    readInstruction();
    instruction = 0;
    while(instruction < 10){
	

	start();
        instruction+=1;
    }

    return 0;
}
