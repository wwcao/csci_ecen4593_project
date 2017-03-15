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
	unsigned int aluSrc1;
	unsigned int aluSrc2;
	unsigned int aluResult;
	alu_ctl aluCtl;
	
	exmem_reg.branchAddr = (idex_reg.extendValue << 2)
															+ idex_reg.nextPC;
	exmem_reg.rd = idex_reg.regDst?idex_reg.rd:	// true rd
																	idex_reg.rt; // else rt
	
	// ALU sources
	aluSrc1 = idex_reg.reg1Value; // expected 3-way mux
	aluSrc2 = idex_reg.aluSrc?idex_reg.extendValue: // true extened value
																	idex_reg.reg2Value; // else reg2
	// ALU Controll Input
	aluResult = 0;
	switch(idex_reg.aluOP) {
		case FORMAT_R: {
			switch (idex_reg.imm&FN_MASK) {
				case R_ADD:
					aluResult = (int)aluSrc1 + (int)aluSrc2;
					break;
				case R_ADDU:
					aluSrc2 = aluSrc2&0x0000ffff;
					aluResult = aluSrc1 + aluSrc2;
				case R_SUB:
					aluResult = (int)aluSrc1 - (int)aluSrc2;
					break;
				case R_AND:
					aluResult = (int)aluSrc1 & (int)aluSrc2;
					break;
				case R_OR:
					aluResult = (int)aluSrc1 | (int)aluSrc2;
					break;
				case R_SLT:
					aluResult = (int)aluSrc1 - (int)aluSrc2;
					exmem_reg.zero = (int)aluResult < 0? 1:0;
					break;
				case R_SLL:
					aluResult = ((int)aluSrc1)<<idex_reg.shamt;
					break;
			}
		}
		case FORMAT_I: {
			aluResult = aluSrc1 + aluSrc2;
		}
		case FORMAT_J: {
			aluResult = aluSrc1 - aluSrc2;
		}
	}
	exmem_reg.aluResult = aluResult;
}

void MEM(void) {
	memwb_reg.exValue = exmem_reg.aluResult;
	unsigned int addr = (exmem_reg.aluResult) >> 2;
	if(exmem_reg.memWrite) {
		memory[addr] = exmem_reg.writeData;
	}
	if(exmem_reg.memRead) {
		memwb_reg.memValue = memory[addr];
	}
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
