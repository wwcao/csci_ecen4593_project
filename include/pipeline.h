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
	
}

void ID(void) {

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
  ID();
  EX();
  MEM();
  WB();
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
