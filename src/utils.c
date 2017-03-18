#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int readInstruction() {
    unsigned int counter;
    char buffer[INSTRUCTION_LEN*2+4];
    FILE* f;
    if(!(f = fopen(INSTRUCTION_PATH, "r"))) Error("Failed to read file\n");
    counter = 0;
    while(fgets(buffer, INSTRUCTION_LEN*2+4, f)
            && buffer[0] != '\n') {
        char addr_s[24];
        int addr;
        int ins;
        sscanf(buffer, "%s%x", addr_s, &ins);
        if(addr_s[1] == 'x')
					addr_s[10] = '\0';
				else
					addr_s[8] = '\0';
        sscanf(addr_s, "%x", &addr);
        if(addr > INS_END_POS) Error("Out of Bound\n");
        memory[addr>>2] = ins;
        //printf("[%s]\tchunk[%d], 0x%x [0x%x]\n", buffer, addr>>2, ins, memory[addr>>2]);
        counter++;
    }
    fclose(f);
    return counter;
}

op_format getInsFormat(int ins) {
    if((ins&OP_MASK) == 2 || (ins&OP_MASK) == 3){
      // 0x02 0x03
      printf("0x%x is J Format\n",ins);
      return FORMAT_J;
    }else if(ins&OP_MASK){
        // non-zero I Type
        printf("0x%x is I Format\n", ins);
        return FORMAT_I;
    }

    printf("0x%x is R Format\n", ins);
    return FORMAT_R;
}

unsigned getRegNum(int ins, reg_type rtype) {
	switch(rtype) {
		case REG_OP:
			return (ins&OP_MASK)>>OP_SHF;
		case REG_RS:
			return (ins&RS_MASK)>>RS_SHF;
		case REG_RT:
			return (ins&RT_MASK)>>RT_SHF;
		case REG_RD:
			return (ins&RD_MASK)>>RD_SHF;
		case REG_SHM:
			return (ins&SH_MASK)>>SHM_SHF;
		case REG_FUNC:
			return (ins&FN_MASK);
		case REG_IMM:
			return (ins&IM_MASK);
		default:
			Error("Instruction: Wrong format\n");
			exit(1);
	}
}

void Error(const char* msg) {
    printf("%s", msg);
    exit(1);
}

void Log(const char* format, ...) {
  // log 
  /*
  va_list argptr = NULL;
  va_start(argptr, format);
  vfprintf(format, argptr);
  va_end(argptr);
  */
  return;
}
