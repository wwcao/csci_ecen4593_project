#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"

int memory[MEMORY_SIZE];
int main() {
    readInstruction();
    printf("type: %d\n", getInsType(memory[0]));
    printf("rs: %d\n", getRegNum(memory[0], REG_RS));

    return 0;
}
