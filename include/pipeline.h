#include "static.h"
#include "units.h"
#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

#define IS_PIPELINE             0
#define RUNTOEND                0

unsigned int memory[MEMORY_SIZE];
unsigned int register_file[REGISTER_NUM];
IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

void IF(void) {

	ifid_reg.ins = instruction;
	ifid_reg.pc = instruction<<2;
	ifid_reg.nextPC = pc+4;
	//ifid_reg.flush = 0;
	
	unsigned int i = ifid_reg.ins;
        
	op_format instFormat = getInsFormat(memory[i]);
        printf("type: %d\n", instFormat);
        switch(instFormat){
         case FORMAT_I:
		ifid_reg.OpCode = getRegNum(memory[i],REG_OP);
		ifid_reg.rs = getRegNum(memory[i],REG_RS);
		ifid_reg.rt = getRegNum(memory[i], REG_RT);
		ifid_reg.immediate = getRegNum(memory[i],REG_IMM);
	
                printf("OpCode: %d\n", ifid_reg.OpCode);
                printf("rs: %d\n", ifid_reg.rs);
                printf("rt: %d\n", ifid_reg.rt);
                printf("immediate: %d\n", ifid_reg.immediate);
         case FORMAT_J:
                ifid_reg.OpCode = getRegNum(memory[i],REG_OP);
                ifid_reg.immediate = getRegNum(memory[i],REG_IMM);

                printf("OpCode: %d\n", getRegNum(memory[i], REG_OP));
                printf("branch target: %d", getRegNum(memory[i], REG_TARGET));
         default://FORMAT_R
		ifid_reg.OpCode = getRegNum(memory[i],REG_OP);
		ifid_reg.rs = getRegNum(memory[i],REG_RS);
                ifid_reg.rt = getRegNum(memory[i], REG_RT);
		ifid_reg.rd = getRegNum(memory[i], REG_RD);
		ifid_reg.shamt = getRegNum(memory[i], REG_SHM);
		ifid_reg.func = getRegNum(memory[i], REG_FUNC);
		

                printf("rs: %d\n", getRegNum(memory[i], REG_RS));
                printf("rt: %d\n", getRegNum(memory[i], REG_RT));
                printf("rd: %d\n", getRegNum(memory[i], REG_RD));
                printf("shamt: %d\n", getRegNum(memory[i], REG_SHM));
                printf("func: %d\n", getRegNum(memory[i], REG_FUNC);
        } 



}

void ID(void) {

 // Control Signal
 switch(ifid_reg.OpCode){
	// case R-format
	case 0:
		idex_reg.regWrite = '1';
 		idex_reg.MemToReg = '0';
		idex_reg.branch = '0';
		idex_reg.memRead = '0';
		idex_reg.memWrite = '0';
		idex_reg.regDst = '1';
		idex_reg.aluOP = '10';
		idex_reg.aluSrc = '0';
		break;
	// case LW
	case 35:
                idex_reg.regWrite = '1';
                idex_reg.MemToReg = '1';
                idex_reg.branch = '0';
                idex_reg.memRead = '1';
                idex_reg.memWrite = '0';
                idex_reg.regDst = '0';
                idex_reg.aluOP = '00';
                idex_reg.aluSrc = '1';
		break;
	// case SW
	case 43:
                idex_reg.regWrite = '0';
                idex_reg.MemToReg = 'x';
                idex_reg.branch = '0';
                idex_reg.memRead = '0';
                idex_reg.memWrite = '1';
                idex_reg.regDst = 'x';
                idex_reg.aluOP = '00';
                idex_reg.aluSrc = '1';
		break;
	// case BEQ
	case 4:
                idex_reg.regWrite = '0';
                idex_reg.MemToReg = 'x';
                idex_reg.branch = '1';
                idex_reg.memRead = '0';
                idex_reg.memWrite = '0';
                idex_reg.regDst = 'x';
                idex_reg.aluOP = '01';
                idex_reg.aluSrc = '0';
		break;
	// case J-format
	case 2 || 3:
                idex_reg.regWrite = '0';
                idex_reg.MemToReg = 'x';
                idex_reg.branch = '0';
                idex_reg.memRead = '0';
                idex_reg.memWrite = '0';
                idex_reg.regDst = 'x';
                idex_reg.aluOP = 'xx';
                idex_reg.aluSrc = 'x';
		break;
}

 idex_reg.nextPC = ifid_reg.nextPC;

 //idex_reg.reg1Value?
 //idex_reg.reg2Value?

 idex_reg.rs = ifid_reg.rs;
 idex_reg.rt = ifid_reg.rt;
 idex_reg.rd = ifid_reg.rd; // need to identify about R-format
 
 // I-format
 idex_reg.extendValue = ifid_reg.immediate>>16; //need help- not sure-immediate is 16bit, signext is 32bit

 // J-format
 idex_reg.bracnAddr = (ifid_reg.target<<2); // need to mask nextPC first 4 bit and add to in front of the shifted target
}

void EX(void) {
	// Based on figure 4.41
	unsigned int aluSrc2;
	
	exmem_reg.branchAddr = (idex_reg.extendValue << 2)
															+ idex_reg.nextPC;
	exmem_reg.rd = idex_reg.regDst?idex_reg.rd:	// true rd
																	idex_reg.rt; // else rt
	aluSrc2 = idex_reg.aluSrc?idex_reg.extendValue: // true extened value
																	idex_reg.reg2Value; // else reg2
	// ALU
	if (idex_reg.aluOP == 0x2) {
		switch (idex_reg.imm&FN_MASK) {
			case R_ADD:
				exmem_reg.aluResult = idex_reg.reg1Value + aluSrc2;
				break;
			case R_SUB:
				exmem_reg.aluResult = idex_reg.reg1Value - aluSrc2;
				break;
			case R_AND:
				exmem_reg.aluResult = idex_reg.reg1Value & aluSrc2;
				break;
			case R_OR:
				exmem_reg.aluResult = idex_reg.reg1Value | aluSrc2;
				break;
			case R_SLT:
				exmem_reg.aluResult = idex_reg.reg1Value - aluSrc2;
				exmem_reg.zero = (int)exmem_reg.aluResult < 0? 1:0;
				break;
			// more operation SLL
		}
	}
	else if(idex_reg.aluOP == 0x1) {
		// Branch Equal
		exmem_reg.aluResult = idex_reg.reg1Value - aluSrc2;
	}
	else {
		// LW SW
		exmem_reg.aluResult = idex_reg.reg1Value + aluSrc2;
	}
}

void MEM(void) {
	
}

void WB(void) {
	unsigned int writedata;
	writedata = memwb_reg.MemtoReg? memwb_reg.memValue:
																	memwb_reg.exValue;
	if(memwb_reg.regWrite)
		// forwarding unit operation should be put here
		register_file[memwb_reg.rd] = writedata;	
}

void start(void) {
	printf("Enter pipeline\n");
  IF();
  WB();
  ID();
  EX();
  MEM();
}								



// controll

void ctlUnitOperation(void) {
  return;
}

void hdUnitOperation(void) {
  return;
}

void fwdUnitOperation(void) {
  return;
}

#endif
