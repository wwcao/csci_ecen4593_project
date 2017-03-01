#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"

int memory[MEMORY_SIZE];
int main() {
    readInstruction();
    printf("type: %d\n", getInsFormat(memory[0]));
    printf("rs: %d\n", getRegNum(memory[0], REG_RS));
    printf("\n");
    printf("type: %d\n", getInsFormat(memory[2]));
    printf("rs: %d\n", getRegNum(memory[2], REG_RS));
    printf("rd: %d\n", getRegNum(memory[2], REG_RD));
    printf("rt: %d\n", getRegNum(memory[2], REG_RT));

    return 0;
}
