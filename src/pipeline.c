#include "pipeline.h"

void IF(void) {
	// handle is pipeline
	if((!IS_PIPELINE) && (cStage != STAGE_IF)) {
		return;
	}
	// IF Operation
	if(PC==0) {
    run_pipeline--;
    return;
	}

	if(Flush_if || Stall) return;

	if(!init_ins && PCSrc) {
		PC = pcSrc2;
		PCSrc = false;
	}
	else if(!init_ins && !PCSrc) {
		PC = pcSrc1;
	}

	if(init_ins)init_ins = false;

	_ifid_reg.progCounter = PC;
	_ifid_reg.instruction = memory[PC];

	pcSrc1 = PC+1;
	_ifid_reg.nPC = pcSrc1;

  if(PC == 12)
    printf("stop");
	// set state ID after IF
	nStage = STAGE_ID;
}

void ID(void) {
	unsigned int instruction;
	unsigned short rs, rd, rt;
	unsigned int opCode, imm;
	unsigned int extendedValue;
	unsigned int regVal1, regVal2;

	// handle is pipeline
	if((!IS_PIPELINE) && (cStage != STAGE_ID)) {
		return;
	}

  if(ifid_reg.progCounter == 81)
    printf("Stop");

	// ID Operation
	memset(&_idex_reg, 0, sizeof(IDEX_Register));

	//printf("\nStage ID, ");
	_idex_reg.progCounter = ifid_reg.progCounter;
	//printf("Operating: [%04d]0x%08x\n", ifid_reg.progCounter, memory[ifid_reg.progCounter]);

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


  regVal1 = register_file[rs];
  regVal2 = register_file[rt];

  // small forwarding
  fwdUnitID(rs, rt, &regVal1, &regVal2);

	ctlUnitOperation(opCode, regVal1, regVal2, extendedValue);

  if(ifid_reg.progCounter == 12)
    printf("stop");
	nStage = STAGE_EX;
}

void EX(void) {
	unsigned src1, src2;

	if((!IS_PIPELINE) && (cStage != STAGE_EX)) {
		return;
	}


	_exmem_reg.progCounter = idex_reg.progCounter;

  src1 = idex_reg.regValue1;
  src2 = idex_reg.regValue2;

	fwdUnitEX(&src1, &src2);

	_exmem_reg.RegWrite = idex_reg.RegWrite;
	_exmem_reg.MemtoReg = idex_reg.MemtoReg;

	_exmem_reg.MemRead = idex_reg.MemRead;
	_exmem_reg.MemWrite = idex_reg.MemWrite;
	_exmem_reg.Branch = idex_reg.Branch;

	_exmem_reg.MemtoReg = idex_reg.MemtoReg;
	_exmem_reg.dataToMem = src2;

	_exmem_reg.rd = idex_reg.RegDst?idex_reg.rd:idex_reg.rt;

	src2 = idex_reg.ALUSrc?idex_reg.extendedValue:src2;
	aluUnitOperation(src1, src2);

	// set next state
	nStage = STAGE_MEM;
	if(idex_reg.progCounter == 12)
    printf("stop");
	return;
}

void MEM(void) {
	unsigned int addr;
	if((!IS_PIPELINE) && (cStage != STAGE_MEM)) {
		return;
	}

	_memwb_reg.progCounter = exmem_reg.progCounter;

	// MEM Operation
	_memwb_reg.RegWrite = exmem_reg.RegWrite;
	_memwb_reg.MemtoReg = exmem_reg.MemtoReg;
	_memwb_reg.aluResult = exmem_reg.aluResult;
	_memwb_reg.rd = exmem_reg.rd;
	addr = exmem_reg.aluResult>>2;

	if(exmem_reg.MemWrite) {
    if(addr > 16384) {
      printf("Out Of Index\n");
    }
		memory[addr] = exmem_reg.dataToMem;
		printf("Out Of Index\n");
	}
	if(exmem_reg.MemRead) {
		_memwb_reg.memValue = memory[addr];
	}

	nStage = STAGE_WB;
}

void WB(void) {

	if((!IS_PIPELINE) & (cStage != STAGE_WB)) {
		return;
	}

	writedata = memwb_reg.MemtoReg? memwb_reg.memValue:
																	memwb_reg.aluResult;

	if(memwb_reg.RegWrite)
		register_file[memwb_reg.rd] = writedata;

	if(!IS_PIPELINE)
		printRegisters();
	nStage = STAGE_IF;

	return;
}

void start(void) {

  hdUnitOperation();
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

  /*
	if(IS_PIPELINE)
		printRegisters();
		*/
}



// controll
void aluUnitOperation(unsigned int src1, unsigned int src2) {
	unsigned result;
	unsigned zero;
	unsigned shamt;

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
						result = (int)src1 & (int)(src2&IM_MASK);
						break;
					case I_ORI:
						result = src1 | (src2&IM_MASK);
						break;
					case I_SLTI:
						result = (int)src1<(int)src2?1:0;
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
					result = (int)src1 + (int)src2;
					break;
				case R_SUB:
					result = (int)src1 - (int)src2;
					break;
        case R_SUBU:
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
					result = (int)src1<(int)src2?1:0;
					break;
				case R_SLTU:
					result = src1<src2?1:0;
					break;
				case R_SLL:
					result = src2 << shamt;
					break;
				case R_SRA:
					result = ((int)src2) >> shamt;
					break;
				case R_SRL:
					result = src2 >> shamt;
					break;
				case R_NOR:
					result = ~(src1|src2);
					break;
				case R_MOVN:
				  if(src2 != 0) {
            result = src1;
				  }
				  else {
            _exmem_reg.RegWrite = false;
				  }
					break;
        case R_MOVZ:
          if(src2 == 0) {
            result = src1;
				  }
				  else {
            _exmem_reg.RegWrite = false;
				  }
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
					PCSrc = true;
					break;
				case R_MOVN:
					//printf("Decoded: R_MOVEN\n");
					if(regVal2 == 0) break;
				default:
					//printf("Decoded:  Format R\n");
					_idex_reg.RegWrite= true;
					_idex_reg.RegDst = true;
					_idex_reg.ALUOp = ALUOP_R;
					break;
			}
			break;
		}
		// case LW
		/*
		case 0x20:
		case 0x21:

		case 0x24:
		case 0x25:
		case 0x26:
		*/
    case 0x23:
			//printf("Decoded:  Format LW\n");
			_idex_reg.RegWrite = true;
			_idex_reg.MemtoReg = true;
			_idex_reg.MemRead = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case SW
		/*
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2e: // swr
		*/
    case 0x2b:
			//printf("Decoded:  Format SW\n");
			_idex_reg.MemWrite = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case Branch
		case 0x04:
			// BEQ
			if(regVal1 != regVal2) break;

			PCSrc = true;
			pcSrc2 = extendedValue + ifid_reg.nPC;
			break;
		case 0x05:
			// BNE
			if(regVal1 == regVal2) break;
			PCSrc = true;
			pcSrc2 = extendedValue+ifid_reg.nPC;
			break;

    case 0x06:
			// BLEZ
			if(regVal1 > regVal2) break;
			PCSrc = true;
			pcSrc2 = extendedValue+ifid_reg.nPC;
			break;
    case 0x07:
      // BGTZ
      if(regVal1 <= 0) break;
			PCSrc = true;
			pcSrc2 = extendedValue+ifid_reg.nPC;
      break;
		// case J-format
		case 0x3:
			// JAL
			jImm = (ifid_reg.instruction&0x03ffffff)<<2;
			msb = ((ifid_reg.nPC-1)<<2)&0xf0000000;
			pcSrc2 = (jImm|msb)>>2;
			PCSrc = true;
			register_file[31] = (ifid_reg.nPC + 1);

			break;
		case 0x2:
			//printf("Decoded:  Format J, ");
			jImm = (ifid_reg.instruction&0x03ffffff)<<2;
			msb = ((ifid_reg.nPC-1)<<2)&0xf0000000;
			pcSrc2 = (jImm|msb)>>2;
			PCSrc = true;

			break;
    case 0x1:
      switch(_idex_reg.rt) {
        case 0x0:
          //bltz
          if(regVal1 < 0) {
          PCSrc = true;
          pcSrc2 = extendedValue+ifid_reg.nPC;
          }
          break;
        case 0x1:
          // bgez
          if(regVal1 >= 0) {
            PCSrc = true;
            pcSrc2 = extendedValue+ifid_reg.nPC;
          }
          break;
        case 0x10:
          // bltzal
          if(regVal1 < 0) {
            register_file[31] = (ifid_reg.nPC+1);
            PCSrc = true;
            pcSrc2 = extendedValue+ifid_reg.nPC;
          }
        break;
        case 0x11:
          // bgezal
          if(regVal1 >= 0) {
            register_file[31] = (ifid_reg.nPC+1);
            PCSrc = true;
            pcSrc2 = extendedValue+ifid_reg.nPC;
          }
        break;
        }
      break;
		default:
			//printf("Decoded:  Format I\n");
			_idex_reg.RegWrite = true;
			_idex_reg.ALUOp = ALUOP_R;
			_idex_reg.ALUSrc = true;
			break;
	}
}

// TODO: instruction index 15
void hdUnitOperation(void) {
    unsigned int rs, rt;
    bool isBranch;
    unsigned opCode;
    rs = getPartNum(ifid_reg.instruction, PART_RS);
    rt = getPartNum(ifid_reg.instruction, PART_RT);
    isBranch = false;
    opCode = 0;

    if(idex_reg.MemRead &&
        ((idex_reg.rt == rs)||(idex_reg.rt == rt))) {
      Stall = true;
    }

    // stall without forwarding in ID Stage
    opCode = getPartNum(ifid_reg.instruction, PART_OP);
    switch(opCode) {
      case 0x1:
      case 0x4:
      case 0x5:
      case 0x6:
      case 0x7:
        isBranch = true;
        break;
    }

    if(isBranch && idex_reg.RegWrite &&
        (((rs != 0) && (idex_reg.rd == rs)) || ((rt != 0) && (idex_reg.rd == rt)))) {
      Stall = true;
    }

    if(isBranch && exmem_reg.RegWrite &&
        (((rs != 0) && (exmem_reg.rd == rs)) || ((rt != 0) && (exmem_reg.rd == rt)))) {
      Stall = true;
    }

    if(isBranch && opCode && idex_reg.RegWrite &&
        (((rs != 0) && (idex_reg.rt == rs)) || ((rt != 0) && (idex_reg.rd == rt)))) {
      Stall = true;
    }

    return;
}

void fwdUnitID(unsigned int rs, unsigned int rt, unsigned int *src1, unsigned int *src2) {
	forwardA = false;
	forwardB = false;

  return;//  disabled!!

  if(!IS_PIPELINE) return;

  if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==rs)	{
    forwardA = 2;
	}
	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==rt)	{
    forwardB = 2;
	}

	//Double Data MEM hazard
	if(memwb_reg.RegWrite && exmem_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == rs)) && memwb_reg.rd==rs)	{
    forwardA = 1;
	}
	if(memwb_reg.RegWrite && exmem_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == rt)) && memwb_reg.rd==rt)	{
    forwardB = 1;
	}

	if(!forwardA && !forwardB)
    return;

	//forwarding
  switch(forwardA) {
    case 1:
      *src1 = writedata;
      break;
    case 2:
      *src1 = exmem_reg.aluResult;  //
      break;
  }
  switch(forwardB) {
    case 1:
      *src2 = writedata;
      break;
    case 2:
      *src2 = exmem_reg.aluResult;
      break;
  }
  return;
}

void fwdUnitEX(unsigned int *src1, unsigned int *src2) {
	IS_FWDING = false;
	forwardA = false;
	forwardB = false;

	//EX hazard
	if(!IS_PIPELINE) return;

	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==idex_reg.rs)	{
    forwardA = 2;
	}
	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==idex_reg.rt)	{
    forwardB = 2;
	}

	//Double Data MEM hazard
	if(memwb_reg.RegWrite && exmem_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == idex_reg.rs)) && memwb_reg.rd==idex_reg.rs)	{
    forwardA = 1;
	}
	if(memwb_reg.RegWrite && exmem_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == idex_reg.rt)) && memwb_reg.rd==idex_reg.rt)	{
    forwardB = 1;
	}

  if(!forwardA && !forwardB)
    return;

	//forwarding
  switch(forwardA) {
    case 1:
      *src1 = writedata;
      break;
    case 2:
      *src1 = exmem_reg.aluResult;  //
      break;
  }
  switch(forwardB) {
    case 1:
      *src2 = writedata;
      break;
    case 2:
      *src2 = exmem_reg.aluResult;
      break;
  }
  return;
}

void wirtetoPipelineRegs() {

  //printf("\nCopying results:\n");
	if(!Stall) {
    memcpy(&ifid_reg, &_ifid_reg , sizeof(IFID_Register));
    memcpy(&idex_reg, &_idex_reg , sizeof(IDEX_Register));
	}

	memcpy(&exmem_reg, &_exmem_reg , sizeof(EXMEM_Register));
	memcpy(&memwb_reg, &_memwb_reg , sizeof(MEMWB_Register));

	if(IS_PIPELINE) {
		if(Flush_id||Stall) {
			//printf("Flush signel from Cotroll Unit\n");
			idex_reg.MemtoReg = false;
			idex_reg.RegWrite = false;
			idex_reg.MemRead = false;
			idex_reg.MemWrite = false;
			idex_reg.ALUOp = ALUOP_NOP;
			idex_reg.ALUSrc = false;
			idex_reg.opCode = false;
			idex_reg.progCounter = false;
			PCSrc = false;
		}

		if(Flush_ex) {
			exmem_reg.MemtoReg = false;
			exmem_reg.RegWrite = false;
			exmem_reg.MemRead = false;
			exmem_reg.MemWrite = false;
		}

		if(Flush_if)
			//memset(&_ifid_reg, 0, sizeof(IFID_Register));
			memset(&ifid_reg, 0, sizeof(IFID_Register));

		//reset flush
		Flush_if = false;
		Flush_id = false;
		Flush_ex = false;
		Stall = false;
	}
	return;
}


void init_pipeline() {
	PCSrc = false;
	init_ins = true;
	Flush_if = false;
	Flush_id = false;
	Flush_ex = false;
	Stall = false;
	cStage = STAGE_IF;
	nStage = STAGE_IF;
	forwardA = false;
	forwardB = false;
	IS_FWDING = false;
	run_pipeline = 3;
}
