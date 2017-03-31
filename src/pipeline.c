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
	//printf("\nStage IF, ");
	
	if(Flush_if) {
		//printf("Flush IF stage\n");
		return;
	}
	
	if(!init_ins && PCSrc) {
		//printf("select pc from src2\n");
		PC = pcSrc2;
		PCSrc = false;
	}
	else if(!init_ins && !PCSrc) {
		//printf("select pc from src1\n");
		PC = pcSrc1;
	}
	if(init_ins) 
		init_ins = false;
	
	_ifid_reg.progCounter = PC;
	//printf("Operating: [%04d]0x%08x\n", PC, memory[PC]);
	
	_ifid_reg.instruction = memory[PC];
	
	pcSrc1 = PC+1;
	_ifid_reg.nPC = pcSrc1;
	
	// set state ID after IF
	nStage = STAGE_ID;
}

void ID(void) {
	unsigned int instruction;
	unsigned short rs, rd, rt;
	unsigned int opCode, imm;
	unsigned int extendedValue;
	
	// handle is pipeline
	if((!IS_PIPELINE) && (cStage != STAGE_ID)) {
		return;
	}
	
	// ID Operation
	memset(&_idex_reg, 0, sizeof(IDEX_Register));
	
	//printf("\nStage ID, ");
	_idex_reg.progCounter = ifid_reg.progCounter;
	//printf("Operating: [%04d]0x%08x\n", ifid_reg.progCounter, memory[ifid_reg.progCounter]);
	
	
	
	// TODO: change to shadow reg
	
	instruction = ifid_reg.instruction;
  rs = getPartNum(instruction, PART_RS);
  rt = getPartNum(instruction, PART_RT);
  rd = getPartNum(instruction, PART_RD);
  imm = getPartNum(instruction, PART_IMM);
  opCode = getPartNum(instruction, PART_OP);
  
  _idex_reg.rs = rs;
  _idex_reg.rt = rt;
  _idex_reg.rd = rd;
  extendedValue = imm&0x00008000?
											imm|0xffff0000:
											imm&0x0000ffff;
	_idex_reg.extendedValue = extendedValue;
	_idex_reg.regValue1 = register_file[rs];
	_idex_reg.regValue2 = register_file[rt];
  _idex_reg.opCode = opCode;

  /*
  printf("\nopCode[%d]\n", opCode);
  printf("rs:%d, rt:%d, rd%d\n", rs, rt, rd);
  printf("imm[%d][0x%x]\n", imm, imm);
  printf("imm&mask[%d][0x%x]\n", imm&0x00008000, imm&0x00008000);
  printf("extendedValue[%d][0x%x]\n\n", 
						_idex_reg.extendedValue, _idex_reg.extendedValue);
  */
	ctlUnitOperation(opCode,register_file[rs], register_file[rt],
										extendedValue);
	
	nStage = STAGE_EX;
}

void EX(void) {
	// Based on figure 4.41
	
	if((!IS_PIPELINE) && (cStage != STAGE_EX)) {
		return;
	}
	//printf("\nStage EX, ");
	_exmem_reg.progCounter = idex_reg.progCounter;
	//printf("Operating: [%04d]0x%08x\n", idex_reg.progCounter, memory[idex_reg.progCounter]);

	fwdUnitOperation();

	_exmem_reg.RegWrite = idex_reg.RegWrite;
	_exmem_reg.MemtoReg = idex_reg.MemtoReg;
	
	_exmem_reg.MemRead = idex_reg.MemRead;
	_exmem_reg.MemWrite = idex_reg.MemWrite;
	_exmem_reg.Branch = idex_reg.Branch;
	
	_exmem_reg.MemtoReg = idex_reg.MemtoReg;
	_exmem_reg.dataToMem = idex_reg.regValue2;
	
	_exmem_reg.rd = idex_reg.RegDst>0?idex_reg.rd:idex_reg.rt;
	aluUnitOperation();
	
	// set next state
	nStage = STAGE_MEM;
}

void MEM(void) {
	unsigned int addr;
	if((!IS_PIPELINE) && (cStage != STAGE_MEM)) {
		return;
	}
	//printf("\nStage MEM, ");
	
	_memwb_reg.progCounter = exmem_reg.progCounter;
	//printf("Operating: [%04d]0x%08x\n", exmem_reg.progCounter, memory[exmem_reg.progCounter]);
	
	// MEM Operation
	_memwb_reg.RegWrite = exmem_reg.RegWrite;
	_memwb_reg.MemtoReg = exmem_reg.MemtoReg;
	_memwb_reg.aluResult = exmem_reg.aluResult;
	_memwb_reg.rd = exmem_reg.rd;
	addr = exmem_reg.aluResult>>2;

	if(exmem_reg.MemWrite) {
		//printf("Writing, addr[%d]\n", addr);
		memory[addr] = exmem_reg.dataToMem;
	}
	if(exmem_reg.MemRead) {
		//printf("Loading, addr[%d]\n", addr);
		_memwb_reg.memValue = memory[addr];
	}
	
	// set next state
	nStage = STAGE_WB;
}

void WB(void) {
	
	unsigned int writedata;
	
	if((!IS_PIPELINE) & (cStage != STAGE_WB)) {
		return;
	}
	//printf("\nStage WB, ");
	//printf("Operating: [%04d]0x%08x\n", memwb_reg.progCounter, memory[memwb_reg.progCounter]);
	// WB Operation 
	writedata = memwb_reg.MemtoReg? memwb_reg.memValue:
																	memwb_reg.aluResult;
	if(memwb_reg.RegWrite)
		register_file[memwb_reg.rd] = writedata;
		
	//printf("register: %d, value: %d\n", memwb_reg.rd, writedata);
	
	if(!IS_PIPELINE)
		printRegisters();
	nStage = STAGE_IF;
}

void start(void) {
  IF();
  
  if(IS_PIPELINE)
		WB();
  
  ID();
  EX();
  MEM();
	
	if(!IS_PIPELINE)
		WB();
	
	wirtetoPipelineRegs();
	cStage = nStage;
	
	if(IS_PIPELINE)
		printRegisters();
	
	/*
	if(PC > 20 && PC < 50) {
		printf("Enter key...\n");
		getchar();
	}
	*/
}						



// controll
void aluUnitOperation(void) {
	unsigned result;
	unsigned zero;
	unsigned shamt;
	
	//forwarding
	
	unsigned src1 = IS_FWDING?(forwardA?memwb_reg.rd:exmem_reg.rd):idex_reg.regValue1;
	unsigned src2 = IS_FWDING?
											(forwardB?
													memwb_reg.rd:exmem_reg.rd):
											(idex_reg.ALUSrc?
													idex_reg.extendedValue:idex_reg.regValue2);
	
	result = 0;		
	zero = 0;	
	//printf("src1[%d], src2[%d]\n", src1, src2);
	shamt = getPartNum(idex_reg.extendedValue, PART_SHM);
	if(idex_reg.ALUOp == ALUOP_LWSW) {
		result = src1 + src2;
	}
	else if(idex_reg.ALUOp == ALUOP_R) {
		if(idex_reg.opCode) {
			// I
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
					default:
						printf("Error @ pc: %04d, instruction: [0x%x]\n", idex_reg.progCounter, memory[idex_reg.progCounter]);
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
					// OR and MOV
				case R_OR:
					result = src1 | src2;
					break;
				case R_XOR:
					result = src1^src2;
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
					result = ((int)src1) >> shamt;
					break;
				case R_SRL:
					result = src1 >> shamt;
					break;
				case R_NOR:
					result = ~(src1|src2);
					break;
				case R_MOVN:
					result = src2 != 0? src1:0;
					break;
				default:
					printf("Error @ pc: %04d, instruction: [0x%x]\n", idex_reg.progCounter, memory[idex_reg.progCounter]);
					exit(1);
			}
		}
	}
	_exmem_reg.zero = zero;
	_exmem_reg.aluResult = result;
	//printf("zero[%d], aluResult[%d]\n", zero, result);
}

void ctlUnitOperation(unsigned int opCode,
							unsigned int regVal1, unsigned regVal2,
							unsigned int extendedValue) {
	unsigned int jImm;
	unsigned int msb;
	_idex_reg.ALUOp = ALUOP_NOP;
	switch(opCode) {
	// case R-format
		case 0x0: {
			unsigned int func = extendedValue&FN_MASK;
			switch (func) {
				case J_R:
					//printf("Decoded:  Format R\n");
					pcSrc2 = regVal1;
					PCSrc = 1;
					if(IS_PIPELINE) {
						//Flush_if = true;
						//Flush_id = true;
						//Flush_ex = true;
					}
					break;
				case R_MOVN:
					//printf("Decoded: R_MOVEN\n");
					if(regVal2 == 0) break;
				default:
					//printf("Decoded:  Format R\n");
					_idex_reg.RegWrite= 1;
					_idex_reg.RegDst = 1;
					_idex_reg.ALUOp = ALUOP_R;
					break;
			}
			break;
		}
		// case LW
		case 0x20:
		case 0x21:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
			//printf("Decoded:  Format LW\n");
			_idex_reg.RegWrite = 1;
			_idex_reg.MemtoReg = 1;
			_idex_reg.MemRead = 1;
			_idex_reg.ALUSrc = 1;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case SW
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2e: // swr
			//printf("Decoded:  Format SW\n");
			_idex_reg.MemWrite = 1;
			_idex_reg.ALUSrc = 1;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case Branch
		case 0x04:
			//printf("Decoded:  Format BEQ\n");
			if(regVal1 != regVal2) break;
			
			PCSrc = 1;
			pcSrc2 = extendedValue + ifid_reg.nPC;
			if(IS_PIPELINE) {
				//Flush_if = true;
				//Flush_id = true;
				//Flush_ex = true;
			}
			break;
		case 0x05:
			//printf("Decoded:  Format BNE\n");
			if(regVal1 == regVal2) break;
			
			PCSrc = 1;
			pcSrc2 = extendedValue+ifid_reg.nPC;
			if(IS_PIPELINE) {
				//Flush_if = true;
				//Flush_id = true;
				//Flush_ex = true;
			}
			//printf("pcSrc2[0x%x]\n", pcSrc2);
			break;
		// case J-format
		case 0x3:
			//printf("Decoded:  Format JAL, ");
			jImm = (ifid_reg.instruction&0x03ffffff)<<2;
			msb = ((ifid_reg.nPC-1)<<2)&0xf0000000; 
			pcSrc2 = (jImm|msb)>>2;
			PCSrc = 1;
			register_file[31] = (ifid_reg.nPC + 1);
			if(IS_PIPELINE) {
				//Flush_if = true;
				//Flush_id = true;
				//Flush_ex = true;
			}
			//printf("pcSrc2[0x%8x]\n", pcSrc2);
			break;
		case 0x2:
			//printf("Decoded:  Format J, ");
			jImm = (ifid_reg.instruction&0x03ffffff)<<2;
			msb = ((ifid_reg.nPC-1)<<2)&0xf0000000; 
			pcSrc2 = (jImm|msb)>>2;
			PCSrc = 1;
			if(IS_PIPELINE) {
				//Flush_if = true;
				//Flush_id = true;
				//Flush_ex = true;
			}
			//printf("pcSrc2[0x%8x]\n", pcSrc2);
			break;
		default:
			//printf("Decoded:  Format I\n");
			_idex_reg.RegWrite = 1;
			_idex_reg.ALUOp = ALUOP_R;
			_idex_reg.ALUSrc = 1; 
			break;
	}
}

// NOT USED
void brhdUnitOperation(unsigned int instruction) {
	
  return;
}

void fwdUnitOperation(void) {
	IS_FWDING = 0;
	forwardA = 0;
	forwardB = 0;
	//EX hazard
	
	if(!IS_PIPELINE) return;
	
	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==idex_reg.rs)	{
        forwardA = 0;
	IS_FWDING = 1;
	} 
	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==idex_reg.rt)	{
        forwardB = 0;
	IS_FWDING = 1;
	} 

	//Double Data MEM hazard
	if(memwb_reg.RegWrite && exmem_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == idex_reg.rs)) && memwb_reg.rd==idex_reg.rs)	{
        forwardA = 1;
	IS_FWDING = 1;
	} 
	if(memwb_reg.RegWrite && exmem_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == idex_reg.rt)) && memwb_reg.rd==idex_reg.rt)	{
        forwardB = 1;
	IS_FWDING = 1;
	} 
	
	//if(IS_FWDING) printf("\n>>>>Forwarding detected<<<<\n");
}

void wirtetoPipelineRegs() {
	if(IS_PIPELINE) {
		if(Flush_id) {
			//printf("Flush signel from Cotroll Unit\n");
			idex_reg.MemtoReg = 0;
			idex_reg.RegWrite = 0;
			idex_reg.MemRead = 0;
			idex_reg.MemWrite = 0;
			idex_reg.ALUOp = ALUOP_NOP;
			idex_reg.ALUSrc = 0;
			idex_reg.opCode = 0;
		}
		
		if(Flush_ex) {
			exmem_reg.MemtoReg = 0;
			exmem_reg.RegWrite = 0;
			exmem_reg.MemRead = 0;
			exmem_reg.MemWrite = 0;
		}
		
		if(Flush_if)
			//memset(&_ifid_reg, 0, sizeof(IFID_Register));
			memset(&ifid_reg, 0, sizeof(IFID_Register));

		//reset flush
		Flush_if = false;
		Flush_id = false;
		Flush_ex = false;
	}
	
	//printf("\nCopying results:\n");
	memcpy(&ifid_reg, &_ifid_reg , sizeof(IFID_Register));
	memcpy(&idex_reg, &_idex_reg , sizeof(IDEX_Register));
	memcpy(&exmem_reg, &_exmem_reg , sizeof(EXMEM_Register));
	memcpy(&memwb_reg, &_memwb_reg , sizeof(MEMWB_Register));
	return;
}


void init_pipeline() {
	PCSrc = false;
	init_ins = true;
	Flush_if = false;
	Flush_id = false;
	Flush_ex = false;
	cStage = STAGE_IF;
	nStage = STAGE_IF;
	forwardA = 0;
	forwardB = 0;
	IS_FWDING = 0;
}
