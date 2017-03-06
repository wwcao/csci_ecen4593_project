
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readInstruction() {
    unsigned counter;
    char buffer[INSTRUCTION_LEN*2];
    FILE* f;
    if(!(f = fopen(INSTRUCTION_PATH, "r"))) Error("Failed to read file\n");
    counter = 0;
    while(fgets(buffer, INSTRUCTION_LEN*2, f)
            && buffer[0] != '\n') {
        char addr_s[24];
        int addr;
        int ins;
        sscanf(buffer+2, "%s%x", addr_s, &ins);
        addr_s[8] = '\0';
        sscanf(addr_s, "%x", &addr);
        buffer[22]='\0';
        if(addr > MEMORY_SIZE) Error("Out of Bound\n");
        memory[addr>>2] = ins;
        printf("[%s]\tchunk[%d], 0x%x [0x%x]\n", buffer, addr>>2, ins, memory[addr>>2]);
        counter++;
    }
    fclose(f);
    return counter;
}

op_format getInsFormat(int ins) {
    if(ins&OP_MASK){
        // non-zero I Type
        printf("0x%x is I Format\n", ins);
        return FORMAT_I;
    }
    printf("0x%x is R Format\n", ins);
    return FORMAT_R;
}

unsigned getRegNum(int ins, reg_type rtype) {
    switch(rtype) {
        case REG_RS:
            return (ins&RS_MASK)>>RS_SHF;
        case REG_RT:
            return (ins&RT_MASK)>>RT_SHF;
        case REG_RD:
            return (ins&RD_MASK)>>RD_SHF;
        default:
            Error("Instruction: Wrong format\n");
            return 0;
    }
}

void printInstr(int ins) {
    
}

void Error(const char* msg) {
    printf("%s", msg);
    exit(1);
}
