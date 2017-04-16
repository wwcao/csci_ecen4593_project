#include "pipeline.h"

void IF(void) {
  //unsigned int instruction;

	// handle is pipeline
	if((!PC)||Stall_cachemissed) {
    //run_pipeline--;
    return;
	}

	if((!IS_PIPELINE) && (cStage != STAGE_IF)) {
		return;
	}

	// IF Operation
  //instruction = 0;
	if(Flush_if || Stall_harzard) {
    numNop++;
    return;
	}

	if(!init_ins && PCSrc) {
    numBranch++;
		PC = pcSrc2;
		PCSrc = false;
	}
	else if(!init_ins && !PCSrc) {
		PC = pcSrc1;
	}

	if(init_ins)init_ins = false;

	_ifid_reg.progCounter = PC;

	if(readFromCache(CACHE_I, PC, &(_ifid_reg.instruction)))
    Stall_cachemissed = false;
	else
    Stall_cachemissed = true;

  pcSrc1 = PC+1;
  _ifid_reg.nPC = pcSrc1;

	// set state ID after IF
	nStage = STAGE_ID;
}

void ID(void) {
	unsigned int instruction;
	unsigned short rs, rd, rt;
	unsigned int opCode, imm;
	int extendedValue;
	int regVal1, regVal2;

	// handle is pipeline
	if((!PC)||Stall_cachemissed) {
    return;
	}

	if((!IS_PIPELINE) && (cStage != STAGE_ID)) {
		return;
	}

	// ID Operation
	_idex_reg.progCounter = ifid_reg.progCounter;

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
	_idex_reg.regValue1 = (int)register_file[rs];
	_idex_reg.regValue2 = (int)register_file[rt];
  _idex_reg.opCode = opCode;


  regVal1 = (int)register_file[rs];
  regVal2 = (int)register_file[rt];

  // small forwarding, disabled
  fwdUnitID(rs, rt, &regVal1, &regVal2);

	ctlUnitOperation(opCode, regVal1, regVal2, extendedValue);

	nStage = STAGE_EX;
}

void EX(void) {
	int src1, src2;

  if((!PC)||Stall_cachemissed) {
    return;
	}

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
  _exmem_reg.dataLen = idex_reg.dataLen;

	_exmem_reg.MemtoReg = idex_reg.MemtoReg;
	_exmem_reg.dataToMem = src2;

	_exmem_reg.rd = idex_reg.RegDst?idex_reg.rd:idex_reg.rt;

	src2 = idex_reg.ALUSrc?idex_reg.extendedValue:src2;
	aluUnitOperation(src1, src2);

	// set next state
	nStage = STAGE_MEM;
	return;
}

void MEM(void) {
	unsigned int addr;
	unsigned short offset;
  unsigned short shamt;
  unsigned int data;
  bool Success;

	if((!PC)||Stall_cachemissed) {
    return;
	}

	if((!IS_PIPELINE) && (cStage != STAGE_MEM)) {
		return;
	}

	_memwb_reg.progCounter = exmem_reg.progCounter;

	// MEM Operation
	_memwb_reg.RegWrite = exmem_reg.RegWrite;
	_memwb_reg.MemtoReg = exmem_reg.MemtoReg;
	_memwb_reg.aluResult = exmem_reg.aluResult;
	_memwb_reg.rd = exmem_reg.rd;

	offset = exmem_reg.aluResult;
	addr = ((unsigned int)exmem_reg.aluResult)>>2;

	if(exmem_reg.MemWrite) {
    Success = writeToCache(addr, (unsigned int)exmem_reg.dataToMem, offset, exmem_reg.dataLen);
    if((!missedPenalty)&&!Success) {
      printf("Error MEM_WRITE @ clock: %u, PC: %04d, instruction: [0x%x]\n",
              clock, exmem_reg.progCounter, memory[exmem_reg.progCounter]);
      exit(0);
    }
	}

	if(exmem_reg.MemRead) {
    if(readFromCache(CACHE_D, addr, &data)) {
      switch(exmem_reg.dataLen) {
        case DLEN_W:
          _memwb_reg.memValue = data;
          break;
        case DLEN_B:
          shamt = (3-offset)*8;
          data >>= shamt;
          data = data&0x80?(data|0xffffff00):data&0xff;
          _memwb_reg.memValue = data;
          break;
        case DLEN_BU:
          shamt = (3-offset)*8;
          data = data&0xff;
          _memwb_reg.memValue = data;
          break;
        case DLEN_HW:
          shamt = (1-offset)*16;
          data >>= shamt;
          data = data&0x8000?(data|0xffff0000):data&0xffff;
          _memwb_reg.memValue = data;
          break;
        case DLEN_HWU:
          shamt = (1-offset)*16;
          data = data&0xffff;
          _memwb_reg.memValue = data;
          break;
        default:
          printf("Error MEM_READ @ clock: %u, PC: %04d, instruction: [0x%x]\n",
                      clock, exmem_reg.progCounter, memory[exmem_reg.progCounter]);
          exit(1);
      }
    }
    else {
      Stall_cachemissed = true;
    }
	}

	nStage = STAGE_WB;
}

void WB(void) {
  if((!PC)||Stall_cachemissed) {
    return;
	}
	if((!IS_PIPELINE) & (cStage != STAGE_WB)|| (!PC)) {
		return;
	}

	writedata = memwb_reg.MemtoReg? memwb_reg.memValue:
																	memwb_reg.aluResult;

	if(memwb_reg.RegWrite && (memwb_reg.rd != 0))
		register_file[memwb_reg.rd] = writedata;

	if(!IS_PIPELINE)
		printRegisters();

	nStage = STAGE_IF;

	return;
}

void startPipeline(void) {
  //if(missedPenalty != 0) return;
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

}

// controll
void aluUnitOperation(int src1, int src2) {
	int result;
	unsigned int zero;
	unsigned int shamt;

	result = 0;
	zero = 0;

	shamt = getPartNum(idex_reg.extendedValue, PART_SHM);
	if(idex_reg.ALUOp == ALUOP_LWSW) {
    numLWSW++;
		result = src1 + src2;
	}
	else if(idex_reg.ALUOp == ALUOP_R) {
	  numR_I++;
		if(idex_reg.opCode) {
			// I
			switch(idex_reg.opCode) {
					case I_ADDI:
						result = src1 + src2;
						break;
					case I_ADDIU:
						result = src1 + src2;
						break;
					case I_ANDI:
						result = src1 & (int)(src2&IM_MASK);
						break;
					case I_ORI:
						result = src1 | (int)(src2&IM_MASK);
						break;
					case I_SLTI:
						result = src1<src2?1:0;
						break;
					case I_SLTIU:
						result = src1<src2?1:0;
						break;
          case I_LUI:
            result = idex_reg.extendedValue<<16;
            break;
          case I_SEB:
            // breaking the format
            result = ((int)src2<<24)>>24;
            break;
					default:
						printf("Error I @ clock: %u, PC: %04d, instruction: [0x%x]\n",
                    clock, idex_reg.progCounter, memory[idex_reg.progCounter]);
						exit(1);
			}
		}
		else {
			// R
			unsigned int func = getPartNum(idex_reg.extendedValue, PART_FUNC);
			switch (func) {
				case R_ADD:
					result = src1 + src2;
					break;
				case R_ADDU:
					result = src1 + src2;
					break;
				case R_SUB:
					result = src1 - src2;
					break;
        case R_SUBU:
					result = src1 - src2;
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
					result = src1 < src2?1:0;
					break;
				case R_SLTU:
					result = src1 < src2?1:0;
					break;
				case R_SLL:
					result = src2<<shamt;
					break;
				case R_SRA:
					result = src2>>shamt;
					break;
				case R_SRL:
					result = ((unsigned int)src2)>>shamt;
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
					printf("Error R @ clock: %u, PC: %04d, instruction: [0x%x]\n",
                    clock, idex_reg.progCounter, memory[idex_reg.progCounter]);
					exit(1);
			}
		}
	}
	_exmem_reg.zero = zero;
	_exmem_reg.aluResult = (unsigned int)result;
}

void ctlUnitOperation(unsigned int opCode,
							int regVal1, int regVal2,
							int extendedValue) {
	unsigned int jImm;
	unsigned int msb;
	_idex_reg.ALUOp = ALUOP_NOP;

	switch(opCode) {
	// case R-format
		case 0x0: {
			unsigned int func = extendedValue&FN_MASK;
			switch (func) {
				case J_R:
					pcSrc2 = regVal1;
					PCSrc = true;
					break;
				default:
					// Decoded R
					_idex_reg.RegWrite= true;
					_idex_reg.RegDst = true;
					_idex_reg.ALUOp = ALUOP_R;
					break;
			}
			break;
		}
		// SPECIAL BEGIN
		// case LW
		case 0x20|I_LB:
		  _idex_reg.dataLen = DLEN_B;

		  _idex_reg.RegWrite = true;
			_idex_reg.MemtoReg = true;
			_idex_reg.MemRead = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
    case 0x21|I_LH:
      _idex_reg.dataLen = DLEN_HW;

		  _idex_reg.RegWrite = true;
			_idex_reg.MemtoReg = true;
			_idex_reg.MemRead = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
      break;
    case 0x23|I_LW:
			_idex_reg.RegWrite = true;
			_idex_reg.MemtoReg = true;
			_idex_reg.MemRead = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
    case 0x24|I_LBU:
      _idex_reg.dataLen = DLEN_BU;

		  _idex_reg.RegWrite = true;
			_idex_reg.MemtoReg = true;
			_idex_reg.MemRead = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
      break;
		case 0x25|I_LHU:
      _idex_reg.dataLen = DLEN_HWU;

		  _idex_reg.RegWrite = true;
			_idex_reg.MemtoReg = true;
			_idex_reg.MemRead = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
		  break;
		// case SW
		case 0x28|I_SB:
		  _idex_reg.dataLen = DLEN_B;
		  _idex_reg.MemWrite = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
		  break;
    case 0x29|I_SH:
      _idex_reg.dataLen = DLEN_HW;
		  _idex_reg.MemWrite = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
      break;
    case 0x2b|I_SW:
			_idex_reg.MemWrite = true;
			_idex_reg.ALUSrc = true;
			_idex_reg.ALUOp = ALUOP_LWSW;
			break;
		// case Branch
		case 0x04|I_BEQ:
			if(regVal1 != regVal2) break;
			PCSrc = true;
			pcSrc2 = extendedValue + ifid_reg.nPC;
			break;
		case 0x05|I_BNE:
			if(regVal1 == regVal2) break;
			PCSrc = true;
			pcSrc2 = extendedValue+ifid_reg.nPC;
			break;
    case 0x06|I_BLEZ:
			if(regVal1 > regVal2) break;
			PCSrc = true;
			pcSrc2 = extendedValue+ifid_reg.nPC;
			break;
    case 0x07|I_BGTZ:
      if(regVal1 <= 0) break;
			PCSrc = true;
			pcSrc2 = extendedValue+ifid_reg.nPC;
      break;
		// case J-format
		case 0x3|J_JAL:
			jImm = (ifid_reg.instruction&0x03ffffff)<<2;
			msb = ((ifid_reg.nPC-1)<<2)&0xf0000000;
			pcSrc2 = (jImm|msb)>>2;
			PCSrc = true;
			register_file[31] = (ifid_reg.nPC + 1);
			break;
		case 0x2|J_J:
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
    case 0x1f|I_SEB:
      _idex_reg.rd = getPartNum(ifid_reg.instruction, PART_RD);
			_idex_reg.RegWrite = true;
			_idex_reg.ALUOp = ALUOP_R;
			_idex_reg.RegDst = true;
      break;
    // SPECIAL END
		default:
			// I
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
      Stall_harzard = true;
    }

    // Stall_harzard without forwarding in ID Stage
    opCode = getPartNum(ifid_reg.instruction, PART_OP);
    switch(opCode) {
      case 0x1:
      case 0x4:
      case 0x5:
      case 0x6:
      case 0x7:
        isBranch = true;
        //break;
    }

    if(isBranch && idex_reg.RegWrite &&
        (((rs != 0) && (idex_reg.rd == rs)) || ((rt != 0) && (idex_reg.rd == rt)))) {
      Stall_harzard = true;
    }

    if(isBranch && exmem_reg.RegWrite &&
        (((rs != 0) && (exmem_reg.rd == rs)) || ((rt != 0) && (exmem_reg.rd == rt)))) {
      Stall_harzard = true;
    }

    if(isBranch && opCode && idex_reg.RegWrite &&
        (((rs != 0) && (idex_reg.rt == rs)) || ((rt != 0) && (idex_reg.rd == rt)))) {
      Stall_harzard = true;
    }

    return;
}

void fwdUnitID(unsigned int rs, unsigned int rt, int *src1, int *src2) {
	unsigned short forwardA = false;
	unsigned short forwardB = false;

  return; //  disabled!!

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

void fwdUnitEX(int *src1, int *src2) {
	unsigned short forwardA = false;
	unsigned short forwardB = false;

	//EX hazard
	if(!IS_PIPELINE) return;

	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==idex_reg.rs)	{
    forwardA = 2;
	}
	if(exmem_reg.RegWrite && exmem_reg.rd!=0 && exmem_reg.rd==idex_reg.rt)	{
    forwardB = 2;
	}

	//Double Data MEM hazard
	if(memwb_reg.RegWrite && memwb_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == idex_reg.rs)) && memwb_reg.rd==idex_reg.rs)	{
    forwardA = 1;
	}
	if(memwb_reg.RegWrite && memwb_reg.rd!=0 && !(exmem_reg.RegWrite && (exmem_reg.rd!=0 && exmem_reg.rd == idex_reg.rt)) && memwb_reg.rd==idex_reg.rt)	{
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

  if(Stall_cachemissed||(missedPenalty != 0)) {
    numNop++;

    pcSrc1--;
    memset(&_ifid_reg, 0, sizeof(IFID_Register));
    memset(&_idex_reg, 0, sizeof(IDEX_Register));
    memset(&_exmem_reg, 0, sizeof(EXMEM_Register));
    memset(&_memwb_reg, 0, sizeof(MEMWB_Register));
    Stall_cachemissed = false;
    return;
  }

	if(!Stall_harzard) {
    memcpy(&ifid_reg, &_ifid_reg , sizeof(IFID_Register));
    memcpy(&idex_reg, &_idex_reg , sizeof(IDEX_Register));
	}

	memcpy(&exmem_reg, &_exmem_reg , sizeof(EXMEM_Register));
	memcpy(&memwb_reg, &_memwb_reg , sizeof(MEMWB_Register));

	if(IS_PIPELINE) {
		if(Flush_id||Stall_harzard) {
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
			memset(&ifid_reg, 0, sizeof(IFID_Register));

		//reset flush
		Flush_if = false;
		Flush_id = false;
		Flush_ex = false;
		Stall_harzard = false;
		Stall_cachemissed = false;
	}

	memset(&_ifid_reg, 0, sizeof(IFID_Register));
	memset(&_idex_reg, 0, sizeof(IDEX_Register));
	memset(&_exmem_reg, 0, sizeof(EXMEM_Register));
	memset(&_memwb_reg, 0, sizeof(MEMWB_Register));
	return;
}


void init_pipeline() {
	PCSrc = false;
	init_ins = true;
	Flush_if = false;
	Flush_id = false;
	Flush_ex = false;
	Stall_harzard = false;
	Stall_cachemissed = false;
	cStage = STAGE_IF;
	nStage = STAGE_IF;
	run_pipeline = 3;
	memset(&ifid_reg, 0, sizeof(IFID_Register));
	memset(&idex_reg, 0, sizeof(IDEX_Register));
	memset(&exmem_reg, 0, sizeof(EXMEM_Register));
	memset(&memwb_reg, 0, sizeof(MEMWB_Register));
}
