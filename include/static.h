#ifndef __STATIC_HEADER__
#define __STATIC_HEADER__

#define INSTRUCTION_PATH      "ins_test.ass"
#define INSTRUCTION_LEN       12
#define MAX_READLINE					256

#define MEMORY_SIZE           4096
#define DMEMORY_SIZE          4096
#define IMEMORY_SIZE          4096
#define INS_END_POS           256
#define INS_START_POS         0

#define REGISTER_NUM          32

// MASKes
#define OP_MASK               0xfc000000
#define RS_MASK               0x03e00000
#define RT_MASK               0x001f0000
#define RD_MASK               0x0000f800
#define SH_MASK               0x000007c0
#define FN_MASK               0x0000003f
#define IM_MASK               0x0000ffff
#define TG_MASK		      0x03ffffff

#define PC_MASK		      0xf0000000

// SHIFT
#define OP_SHF              26
#define RS_SHF                21
#define RT_SHF                16
#define RD_SHF                11
#define SHM_SHF               6

//Instruction Op
// I format -- OPcode
#define I_ADDI                0x08
#define I_ADDIU               0x09
#define I_SLTI                0x0a
#define I_SLTIU               0x0b    // For preview TODO: change it back
//#define I_SLTIU
#define I_ANDI	 	      0x0c
#define I_BEQ		      	0x04
#define I_BNE		      	0x05
#define I_LBU		      	0x24
#define I_LHU		      	0x25
#define I_LUI		      	0x0f
#define I_LW		      	0x23
#define I_ORI		      	0x0d
#define I_SD		      	0x28
#define I_SH		      	0x29
#define I_SW		      	0x2b

// R format -- FUNC
#define R_ADD                 0x20    // For preview TODO: change it back
#define R_ADDU		      0x21
#define R_AND		      	0x24
#define R_DIV		      	0x1a
#define R_DIVU		      0x1b
#define R_JR		      	0x08
#define R_MULT	 	      0x18
#define R_MULTU		      0x19
#define R_NOR		      	0x27
#define R_XOR           0x26
#define R_OR            0x25
#define R_SLT		      	0x2a
#define R_SLTU		      0x2b
#define R_SLL		      	0x00
#define R_SRL		      	0x02
#define R_SRA		      	0x03
#define R_SUB		      	0x22
#define R_SUBU                0x23

// J Format -- OpCode
#define J_J		      0x02
#define J_JAL		      0x03

typedef enum {FORMAT_I = 0, FORMAT_R,FORMAT_J} op_format;
typedef enum {PART_OP = 0, PART_RS, PART_RT, PART_RD, PART_SHM, PART_FUNC, PART_IMM} part_type;
typedef enum {ALUOP_LWSW = 0, ALUOP_BEQ, ALUOP_R ,} alu_op;
typedef enum {STAGE_IF=0x0, STAGE_ID, STAGE_EX, STAGE_MEM, STAGE_WB} stage;
typedef enum {false = 0, true = 1} bool;

//
// pipeline register types

typedef struct {
  bool HFlush;
  bool CFlush;
  bool PCWrite;
  unsigned int PC;
  unsigned int instruction;
} IFID_Register;

typedef struct {
	// WB
	bool MemtoReg;
	bool RegWrite;
	// MEM
	bool Branch; // present in 4.51
	bool MemRead;
	bool MemWrite;
	// EX
	bool ALUSrc;
	bool RegDst;
	alu_op ALUOp;
	unsigned opCode;
	
	unsigned int ins; // used for error ONLY
	
	unsigned int PC;
	unsigned int regValue1;
	unsigned int regValue2;
	unsigned int extendedValue;
	
	unsigned short rs;
	unsigned short rt;
	unsigned short rd;
} IDEX_Register;

typedef struct {
  // WB
	bool MemtoReg;
	bool RegWrite;
	// MEM
	bool Branch; // present in 4.51
	bool MemRead;
	bool MemWrite;
	
	unsigned int PC;
	bool zero;
	unsigned int aluResult;
	unsigned int dataToMem;
	unsigned short rd;
} EXMEM_Register;

typedef struct {
  // WB
	bool MemtoReg;
	bool RegWrite;
	
	unsigned int memValue;
	unsigned int aluResult;
	unsigned short rd;
} MEMWB_Register;

typedef struct {
  
} shadow_registers;

#endif
