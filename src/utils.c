#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int readInstruction() {
    unsigned counter;
    char buffer[INSTRUCTION_LEN];
    FILE* f;
    int* p_mem_ins;
    
    if(!(f = fopen(INSTRUCTION_PATH, "r"))) Error("Failed to read file\n");
    
    p_mem_ins = memory + INS_START_POS;
    counter = 0;
    while(fgets(buffer, INSTRUCTION_LEN, f)) {
        sscanf(buffer+2, "%x", p_mem_ins++);
        printf("->0x%x\n", memory[counter]);
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
