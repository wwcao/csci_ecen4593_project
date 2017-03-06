#include <stdio.h>
#include "utils.h"
#include "static.h"
#include "units.h"

int memory[MEMORY_SIZE];
int main() {
    readInstruction();
    int i = 0;
    while(i < 10){
        printf("type: %d\n", getInsFormat(memory[i]));
        printf("rs: %d\n", getRegNum(memory[i], REG_RS));
        printf("rd: %d\n", getRegNum(memory[i], REG_RD));
        printf("rt: %d\n", getRegNum(memory[i], REG_RT));

        i+=1;
    }

    return 0;
}
