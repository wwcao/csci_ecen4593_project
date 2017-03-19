#include <stdlib.h>
#include <strings.h>
#include "static.h"
#include "units.h"
#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

#define IS_PIPELINE             0
#define RUNTOEND                0

unsigned int memory[MEMORY_SIZE];
unsigned int register_file[REGISTER_NUM];
unsigned int pc;
IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;
stage cStage;			// curent state for non_pipeline
stage nStage;			// next state

void IF(void) {
	// handle is pipeline
	if((!IS_PIPELINE) & (cStage != STAGE_IF)) {
		return;
	}
	
	printf("Stage IF\n");
	// IF Operation
	ifid_reg.inst = memory[pc];
	ifid_reg.PC = pc<<2;
	ifid_reg.nextPC = (ifid_reg.PC+4)>>2; // TODO: fix this 
	ifid_reg.isJump = 0;
	ifid_reg.insFormat = getInsFormat(ifid_reg.inst);
	if( ifid_reg.insFormat == FORMAT_J){
	  ifid_reg.isJump = 1; //jump mux
	}
	
	// set state ID after IF
	nStage = STAGE_ID;
}

void ID(void) {
	// handle is pipeline
	if((!IS_PIPELINE) & (cStage != STAGE_ID)) {
		return;
	}
	printf("Stage ID, ");
	memset(&idex_reg, 0, sizeof(IDEX_Register));
	idex_reg.nextPC = ifid_reg.nextPC;
	idex_reg.inst = ifid_reg.inst;
	idex_reg.instFormat = ifid_reg.insFormat;
	// Decode pc
  switch(idex_reg.instFormat){
		case FORMAT_I:
			printf("ID: Format I\n");
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
			printf("ID for Format R 1, ");
			idex_reg.OpCode = getRegNum(ifid_reg.inst,REG_OP);
			idex_reg.rs = getRegNum(ifid_reg.inst, REG_RS);
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
			printf("ID for Format R 2, ");
			idex_reg.regWrite = 1;
			idex_reg.regDst = 1;
			idex_reg.aluOP = 2;

			idex_reg.reg1Value = register_file[idex_reg.rs];
			idex_reg.reg2Value = register_file[idex_reg.rt];
			idex_reg.destination = idex_reg.func?idex_reg.rd:idex_reg.rt;
			break;
		// case LW
		case 35:
			idex_reg.regWrite = 1;
			idex_reg.MemToReg = 1;
			idex_reg.memRead = 1;
			idex_reg.aluSrc = 1;

			idex_reg.reg1Value = register_file[idex_reg.rs];
			idex_reg.reg2Value = register_file[idex_reg.rt];
			idex_reg.destination = idex_reg.rt;
			
			idex_reg.extendValue = idex_reg.immediate&0x00008000?
											(ifid_reg.inst&0xffff0000)|ifid_reg.inst:
											ifid_reg.inst&0x0000ffff;
			break;
		// case SW
		case 43:
			idex_reg.memWrite = 1;
			idex_reg.aluSrc = 1;
			idex_reg.reg1Value = register_file[idex_reg.rs];
			idex_reg.reg2Value = register_file[idex_reg.rt];
			idex_reg.destination = idex_reg.rt;
			idex_reg.extendValue = idex_reg.immediate&0x00008000?
											(ifid_reg.inst&0xffff0000)|ifid_reg.inst:
											ifid_reg.inst&0x0000ffff;
			break;
		// case BEQ
		case 4:
			idex_reg.branch = 1;
			idex_reg.aluOP = 1;

			idex_reg.reg1Value = register_file[idex_reg.rs];
			idex_reg.reg2Value = register_file[idex_reg.rt];
			idex_reg.extendValue = idex_reg.immediate&0x00008000?
											(ifid_reg.inst&0xffff0000)|ifid_reg.inst:
											ifid_reg.inst&0x0000ffff;
			break;
			
		// case J-format
		case 2 || 3:
			idex_reg.jumpTarget = (idex_reg.nextPC&PC_MASK) + (idex_reg.target<<2); // targetJump
			break;
		default:
			printf("Decode of Format I\n");
			idex_reg.regWrite = 1;
			idex_reg.aluOP = 2;
			idex_reg.aluSrc = 1; 
			idex_reg.reg1Value = register_file[idex_reg.rs];
			idex_reg.reg2Value = register_file[idex_reg.rt];
			idex_reg.destination = idex_reg.rt;
			
			idex_reg.extendValue = idex_reg.immediate&0x00008000?
															idex_reg.immediate^0xffff0000:
															idex_reg.immediate^0x00000000;
			break;
	}
	// set state after ID
	nStage = STAGE_EX;
}

void EX(void) {
	// Based on figure 4.41
	char zero;
	unsigned int aluSrc1;
	unsigned int aluSrc2;
	unsigned int aluResult;
	
	if((!IS_PIPELINE) & (cStage != STAGE_EX)) {
		return;
	}
	printf("Stage EX\n");
	// start EX operation
	exmem_reg.regWrite = idex_reg.regWrite;
	exmem_reg.memRead = idex_reg.memRead;
	exmem_reg.memWrite = idex_reg.memWrite;
	exmem_reg.MemToReg = idex_reg.MemToReg;
	
	exmem_reg.branchAddr = (idex_reg.extendValue << 2)
															+ idex_reg.nextPC;
	exmem_reg.rd = idex_reg.destination;
	// ALU sources
	aluSrc1 = idex_reg.reg1Value; // expected 3-way mux
	aluSrc2 = idex_reg.aluSrc?idex_reg.extendValue: // true extened value
																	idex_reg.reg2Value; // else reg2
	// ALU Controll Input
	zero = 0;
	aluResult = 0;
	switch(idex_reg.aluOP) {
		case 0:
			// LWSW
			printf("LWSW in EX, ");
			aluResult = aluSrc1 + aluSrc2;
			printf("aluSrc1[%d],aluSrc2[%d],aluResult[%d]\n", aluSrc1, aluSrc2, aluResult);
			break;
		case 1:
			// BNE
			aluResult = aluSrc1 - aluSrc2;
			break;
		case 2: {
			if(idex_reg.OpCode) {
				printf("EX: Format I, ");
				switch(idex_reg.OpCode) {
					case I_ADDI:
						printf("%d + %d = ", aluSrc1, aluSrc2);
						aluResult = (int)aluSrc1 + (int)aluSrc2;
						printf("%d, ", aluResult);
						break;
					case I_ADDIU:
						aluResult = aluSrc1 + aluSrc2;
						break;
					case I_ANDI:
						aluResult = aluSrc1 & (aluSrc2&IM_MASK);
						break;
					case I_ORI:
						aluResult = aluSrc1 | (aluSrc2&IM_MASK);
						break;
					case I_SLTI:
						aluResult = (int)aluSrc1 - (int)aluSrc2;
						zero = aluResult < 0? 1: 0;
						break;
					case I_SLTIU:
						aluResult = aluSrc1 - aluSrc2;
						zero = aluResult < 0? 1: 0;
						break;
					default:
						printf("Error @ pc: %d, instruction: [0x%x]\n", pc, ifid_reg.inst);
						exit(1);
				}
			}
			else {
				printf("EX: Format R, ");
				switch (idex_reg.immediate&FN_MASK) {
					case R_ADD:
						aluResult = (int)aluSrc1 + (int)aluSrc2;
						break;
					case R_ADDU:
						aluResult = aluSrc1 + aluSrc2;
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
					case R_SLTU:
						aluResult = aluSrc1 - aluSrc2;
						zero = (int)aluResult < 0? 1:0;
						break;
					case R_SLL:
						aluResult = ((int)aluSrc1)<<idex_reg.shamt;
						break;
					case R_SRA:
						aluResult = aluSrc1 >> idex_reg.shamt;
						break;
					case R_SRL:
						aluResult = (aluSrc1 >> idex_reg.shamt)&
													(~(0x80000000>>idex_reg.shamt));
						break;
					case R_NOR:
						aluResult = ~(aluSrc1|aluSrc2);
						break;
					default:
						printf("Error @ pc: %ud, instruction: 0x%x\n", (idex_reg.nextPC<<4)-4, idex_reg.inst);
						exit(1);
				}
			}
		}
	}
	exmem_reg.zero = zero;
	exmem_reg.aluResult = aluResult;
	exmem_reg.writeData = idex_reg.reg2Value;
	// set next state
	nStage = STAGE_MEM;
}

void MEM(void) {
	unsigned int addr;
	if((!IS_PIPELINE) & (cStage != STAGE_MEM)) {
		return;
	}
	printf("Stage MEM, ");
	// MEM Operation
	memwb_reg.regWrite = exmem_reg.regWrite;
	memwb_reg.MemtoReg = exmem_reg.MemToReg;
	memwb_reg.exValue = exmem_reg.aluResult;
	memwb_reg.rd = exmem_reg.rd;
	addr = exmem_reg.aluResult>>2;
	
	if(exmem_reg.memWrite) {
		printf("Writing, addr[%d]\n", addr);
		memory[addr] = exmem_reg.writeData;
	}
	if(exmem_reg.memRead) {
		printf("Loading, addr[%d]\n", addr);
		memwb_reg.memValue = memory[addr];
	}
	// set next state
	nStage = STAGE_WB;
}

void WB(void) {
	
	unsigned int writedata;
	
	if((!IS_PIPELINE) & (cStage != STAGE_WB)) {
		return;
	}
	printf("Stage WB\n");
	// WB Operation 
	writedata = memwb_reg.MemtoReg? memwb_reg.memValue:
																	memwb_reg.exValue;
	if(memwb_reg.regWrite)
		// forwarding unit operation should be put here
		register_file[memwb_reg.rd] = writedata;
	
	// set next state
	nStage = STAGE_IF;
	if(!IS_PIPELINE){
		pc++;
	}
}

void start(void) {
	printf("Start stages\n");
  IF();
  WB();
  ID();
  EX();
  MEM();
  cStage = nStage;
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
