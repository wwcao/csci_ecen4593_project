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

	ifid_reg.inst = memory[instruction];
	ifid_reg.pc = instruction<<2;
	ifid_reg.nextPC = pc+4;
	ifid_reg.isJump = 0;
	ifid_reg.instFormat = getInsFormat(ifid_reg.inst)
	if( ifid_reg.instFormat == FORMAT_J){
	  ifid_reg.isJump = 1; //jump mux
	}
}

void ID(void) {

 idex_reg.nextPC = ifid_reg.nextPC;
 idex_reg.inst = ifid_reg.inst;
 idex_reg.instFormat = ifid_reg.instFormat;
 // Decode Instruction
        switch(idex_reg.instFormat){
         case FORMAT_I:
                idex_reg.OpCode = getRegNum(ifid_reg.inst,REG_OP);
                idex_reg.rs = getRegNum(ifid_reg.inst,REG_RS);
                idex_reg.rt = getRegNum(ifid_reg.inst, REG_RT);
                idex_reg.immediate = getRegNum(ifid_reg.inst,REG_IMM);
		break;

         case FORMAT_J:
                idex_reg.OpCode = getRegNum(ifid_reg.inst,REG_OP);
                idex_reg.target = getRegNum(ifid_reg.inst,REG_TARGET);
		idex_reg.isJump = ifid_reg.isJump; //jump mux
		break;

         default://FORMAT_R
                idex_reg.OpCode = getRegNum(ifid_reg.inst,REG_OP);
                idex_reg.rs = getRegNum(ifid_reg.inst,REG_RS);
                idex_reg.rt = getRegNum(ifid_reg.inst, REG_RT);
                idex_reg.rd = getRegNum(ifid_reg.inst, REG_RD);
                idex_reg.shamt = getRegNum(ifid_reg.inst, REG_SHM);
                idex_reg.func = getRegNum(ifid_reg.inst, REG_FUNC);
		break;        
	}

 // Control Signal
 // Source and Destination registers
 switch(idex_reg.OpCode){
	// case R-format
	case 0:
		idex_reg.regWrite = 1;
		idex_reg.regDst = 1;
		idex_reg.aluOP = 2;

                idex_reg.reg1Value = idex_reg.rs;
                idex_reg.reg2Value = idex_reg.rt;
                idex_reg.destination = idex_reg.rd;
		break;
	// case LW
	case 35:
                idex_reg.regWrite = 1;
                idex_reg.MemToReg = 1;
                idex_reg.memRead = 1;
                idex_reg.aluSrc = 1;

                idex_reg.reg1Value = idex_reg.rs;
                idex_reg.destination = idex_reg.rt;
                if((1<<15) & value) == 0){
	         idex_reg.extendValue = ifid_reg.immediate;
       	         break;
                }
                idex_reg.extendValue = (((1<<16)-1)<<16) | ifid_reg.immediate;
		break;
	// case SW
	case 43:
                idex_reg.memWrite = 1;
                idex_reg.aluSrc = 1;
		
                idex_reg.reg1Value = idex_reg.rs;
                idex_reg.destination = idex_reg.rt;
                if((1<<15) & value) == 0){
                 idex_reg.extendValue = idex_reg.immediate;
                 break;
                }
                idex_reg.extendValue = (((1<<16)-1)<<16) | ifid_reg.immediate;

		break;
	// case BEQ
	case 4:
                idex_reg.branch = 1;
                idex_reg.aluOP = 1;

                idex_reg.reg1Value = idex_reg.rs;
                idex_reg.reg2Value = idex_reg.rt;
                if((1<<15) & value) == 0){
                 idex_reg.extendValue = idex_reg.immediate;
                 break;
                }
                idex_reg.extendValue = (((1<<16)-1)<<16) | ifid_reg.immediate;

		break;
	// case J-format
	case 2 || 3:
                idex_reg.jumpTarget = (idex_reg.nextPC&PC_MASK) + (idex_reg.target<<2); // targetJump
		break;
	default:
		break;
}

}

void EX(void) {
	// Based on figure 4.41
	char zero;
	unsigned int aluSrc1;
	unsigned int aluSrc2;
	unsigned int aluResult;
	
	exmem_reg.branchAddr = (idex_reg.extendValue << 2)
															+ idex_reg.nextPC;
	exmem_reg.rd = idex_reg.regDst?idex_reg.rd:	// true rd
																	idex_reg.rt; // else rt
	
	// ALU sources
	aluSrc1 = idex_reg.reg1Value; // expected 3-way mux
	aluSrc2 = idex_reg.aluSrc?idex_reg.extendValue: // true extened value
																	idex_reg.reg2Value; // else reg2
	// ALU Controll Input
	zero = 0;
	aluResult = 0;
	switch(idex_reg.aluOP) {
		case FORMAT_R: {
			switch (idex_reg.imm&FN_MASK) {
				case R_ADD:
					aluResult = (int)aluSrc1 + (int)aluSrc2;
					break;
				case R_SUB:
					aluResult = (int)aluSrc1 - (int)aluSrc2;
					break;
				case R_AND:
					aluResult = aluSrc1 & aluSrc2;
					break;
				case R_OR:
					aluResult = aluSrc1 | aluSrc2;
					break;
				case R_SLT:
					aluResult = (int)aluSrc1 - (int)aluSrc2;
					zero = (int)aluResult < 0? 1:0;
					break;
				case R_SLL:
					aluResult = ((int)aluSrc1)<<idex_reg.shamt;
					break;
				case R_NOR:
					aluResult = ~(aluSrc1|aluSrc2);
					break;
			}
		}
		case FORMAT_I: {
			// some arithmetic operations are not basic MIPS Instructions
			aluResult = aluSrc1 + aluSrc2;
		}
		case FORMAT_J: {
			aluResult = aluSrc1 - aluSrc2;
		}
	}
	exmem_reg.zero = zero;
	exmem_reg.aluResult = aluResult;
}

void MEM(void) {
	memwb_reg.exValue = exmem_reg.aluResult;
	unsigned int addr = exmem_reg.aluResult>>2;
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
