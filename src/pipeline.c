#include "pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "static.h"
#include "units.h"
#include "utils.h"

void IF(void) {
	// handle is pipeline
	if((!IS_PIPELINE) && (cStage != STAGE_IF)) {
		return;
	}
	// IF Operation
	printf("\nStage IF\n");
	if(PCSrc) {
		printf("select pc from src2\n");
		PC = pcSrc2;
		PCSrc = 0;
	}
	else {
		printf("select pc from src1\n");
		PC = pcSrc1;
	}
	
	ifid_reg.instruction = memory[PC >> 2];
	printf("Instruction: 0x%x\n", ifid_reg.instruction);
	pcSrc1 = PC+4;
	ifid_reg.PC = PC;
	
	// set state ID after IF
	nStage = STAGE_ID;
}

void ID(void) {
	unsigned int instruction;
	unsigned short rs, rd, rt;
	unsigned int opCode, imm;
	
	// handle is pipeline
	if((!IS_PIPELINE) && (cStage != STAGE_ID)) {
		return;
	}
	
	printf("\nStage ID, ");
	
	// ID Operation
	memset(&idex_reg, 0, sizeof(IDEX_Register));
	
	// TODO: change to shadow reg
	idex_reg.PC = ifid_reg.PC;
	printf("\nPC in reg[0x%x]\n", idex_reg.PC);
	
	instruction = ifid_reg.instruction;
  rs = getPartNum(instruction, PART_RS);
  rt = getPartNum(instruction, PART_RT);
  rd = getPartNum(instruction, PART_RD);
  imm = getPartNum(instruction, PART_IMM);
  opCode = getPartNum(instruction, PART_OP);
  
  idex_reg.rs = rs;
  idex_reg.rt = rt;
  idex_reg.rd = rd;
  idex_reg.extendedValue = imm&0x00008000?
											imm|0xffff0000:
											imm&0x0000ffff;
	idex_reg.regValue1 = register_file[rs];
	idex_reg.regValue2 = register_file[rt];
  idex_reg.opCode = opCode;

  
  printf("\nopCode[%d]\n", opCode);
  printf("rs:%d, rt:%d, rd%d\n", rs, rt, rd);
  printf("imm[%d][0x%x]\n", imm, imm);
  printf("imm&mask[%d][0x%x]\n", imm&0x00008000, imm&0x00008000);
  printf("extendedValue[%d][0x%x]\n\n", idex_reg.extendedValue,idex_reg.extendedValue);
  
	ctlUnitOperation(opCode);
	
	// set state after ID
	nStage = STAGE_EX;
}

void EX(void) {
	// Based on figure 4.41
	
	if((!IS_PIPELINE) && (cStage != STAGE_EX)) {
		return;
	}
	printf("\nStage EX\n");
	
	exmem_reg.RegWrite = idex_reg.RegWrite;
	exmem_reg.MemtoReg = idex_reg.MemtoReg;
	
	exmem_reg.MemRead = idex_reg.MemRead;
	exmem_reg.MemWrite = idex_reg.MemWrite;
	exmem_reg.Branch = idex_reg.Branch;
	
	exmem_reg.MemtoReg = idex_reg.MemtoReg;
	exmem_reg.PC = (idex_reg.extendedValue << 2) + idex_reg.PC;
	printf("---- bran [%d]\n", exmem_reg.Branch);
	printf("---- idex	[0x%x]\n", idex_reg.PC);
	printf("---- exte [0x%x]\n", idex_reg.extendedValue<<2);
	printf("---- Calc [0x%x]\n", exmem_reg.PC);
	exmem_reg.dataToMem = idex_reg.regValue2;
	
	exmem_reg.rd = idex_reg.RegDst>0?idex_reg.rd:idex_reg.rt;
	aluUnitOperation();
	
	// set next state
	nStage = STAGE_MEM;
}

void MEM(void) {
	unsigned int addr;
	if((!IS_PIPELINE) && (cStage != STAGE_MEM)) {
		return;
	}
	printf("\nStage MEM, ");
	// MEM Operation
	
	// Branch
	
	if(exmem_reg.Branch&&exmem_reg.zero) {
		
		pcSrc2 = exmem_reg.PC;
		PCSrc = 1;
	}
	
	memwb_reg.RegWrite = exmem_reg.RegWrite;
	memwb_reg.MemtoReg = exmem_reg.MemtoReg;
	memwb_reg.aluResult = exmem_reg.aluResult;
	memwb_reg.rd = exmem_reg.rd;
	addr = exmem_reg.aluResult>>2;
	printf("Address[%d]\n", addr);
	
	if(exmem_reg.MemWrite) {
		printf("Writing, addr[%d]\n", addr);
		memory[addr] = exmem_reg.dataToMem;
	}
	if(exmem_reg.MemRead) {
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
	printf("\nStage WB\n");
	// WB Operation 
	writedata = memwb_reg.MemtoReg? memwb_reg.memValue:
																	memwb_reg.aluResult;
	if(memwb_reg.RegWrite)
		// forwarding unit operation should be put here
		register_file[memwb_reg.rd] = writedata;
	printf("register: %d, value: %d\n", memwb_reg.rd, writedata);
	// set next state
	/*
	if(!IS_PIPELINE)
		PC = pcSrc1;
		*/
	nStage = STAGE_IF;
}

void start(void) {
  IF();
  ID();
  EX();
  MEM();
  WB();
  if(cStage == STAGE_WB)
		printRegisters();
  cStage = nStage;
  //printf("Enter key...\n");
  //getchar();
}						



// controll
void aluUnitOperation(void) {
	unsigned result;
	unsigned zero;
	unsigned shamt;
	// forwarding should change these two lines
	unsigned src1 = idex_reg.regValue1;
	unsigned src2 = idex_reg.ALUSrc?idex_reg.extendedValue: // true extened value
																	idex_reg.regValue2; // else reg2
	result = 0;		
	zero = 0;	
	printf("src1[%d], src2[%d]\n", src1, src2);
	shamt = getPartNum(idex_reg.extendedValue, PART_SHM);
	if(idex_reg.ALUOp == ALUOP_BEQ) {
		zero = src1==src2?1:0;
		printf("----zero[%d]\n", zero);
	}
	else if(idex_reg.ALUOp == ALUOP_LWSW) {
		result = src1 + src2;
	}
	else {
		if(idex_reg.opCode) {
			// I and J
			switch(idex_reg.opCode) {
					case I_ADDI:
						result = (int)src1 + (int)src2;
						break;
					case I_ADDIU:
						result = src1 + src2;
						break;
					case I_ANDI:
						result = src1 & (src2&IM_MASK);
						break;
					case I_ORI:
						result = src1 | (src2&IM_MASK);
						break;
					case I_SLTI:
						result = src1<src2?1:0;
						break;
					case I_SLTIU:
						result = src1<src2?1:0;
						break;
					case J_J:
					case J_JAL:
						break;
					default:
						printf("Error @ pc: %d, instruction: [0x%x]\n", idex_reg.PC, idex_reg.ins);
						exit(1);
			}
		}
		else {
			// R
			unsigned int func = getPartNum(idex_reg.extendedValue, PART_FUNC);
			switch (func) {
				case R_ADD:
					result = (int)src1 + (int)src2;
					break;
				case R_ADDU:
					result = src1 + src2;
					break;
				case R_SUB:
					result = (int)src1 - (int)src2;
					break;
				case R_AND:
					result = src1 & src2;
					break;
				case R_OR:
					result = src1 | src2;
					break;
				case R_SLT:
					result = src1<src2?1:0;
					break;
				case R_SLTU:
					result = src1<src2?1:0;
					break;
				case R_SLL:
					result = ((int)src1) << shamt;
					break;
				case R_SRA:
					result = src1 >> shamt;
					break;
				case R_SRL:
					result = (src1 >> shamt)&
												(~(0x80000000>>shamt));
					break;
				case R_NOR:
					result = ~(src1|src2);
					break;
				default:
					printf("Error @ pc: %d, instruction: [0x%x]\n", idex_reg.PC, idex_reg.ins);
					exit(1);
			}
		}
	}
	exmem_reg.zero = zero;
	exmem_reg.aluResult = result;
	printf("zero[%d], aluResult[%d]\n", zero, result);
}

void ctlUnitOperation(unsigned int opCode) {
	switch(opCode) {
	// case R-format
		case 0x0:
			printf("Decode of Format R\n");
			idex_reg.RegWrite= 1;
			idex_reg.RegDst = 1;
			idex_reg.ALUOp = ALUOP_R;
			break;
		// case LW
		case 0x20:
		case 0x21:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
			printf("Decode of Format LW\n");
			idex_reg.RegWrite = 1;
			idex_reg.MemtoReg = 1;
			idex_reg.MemRead = 1;
			idex_reg.ALUSrc = 1;
			idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case SW
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2e: // swr
			printf("Decode of Format SW\n");
			idex_reg.MemWrite = 1;
			idex_reg.ALUSrc = 1;
			idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case BEQ
		case 0x04:
		case 0x05:
			printf("Decode of Format BEQ\n");
			idex_reg.Branch = 1;
			idex_reg.ALUOp = ALUOP_BEQ;
			break;
		// case J-format
		case 0x2:
		case 0x3:
			printf("Decode of Format J\n");
			//idex_reg.jumpTarget = (idex_reg.nextPC&PC_MASK) + (idex_reg.target<<2); // targetJump
			break;
		default:
			printf("Decode of Format I\n");
			idex_reg.RegWrite = 1;
			idex_reg.ALUOp = ALUOP_R;
			idex_reg.ALUSrc = 1; 
			break;
	}
}

void hdUnitOperation(void) {
  return;
}

void fwdUnitOperation(void) {
	/*
	 * 1. change alu operation
	 * 2. ...
	 */
  return;
}
