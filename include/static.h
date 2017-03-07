#ifndef __STATIC_HEADER__
#define __STATIC_HEADER__

#define INSTRUCTION_PATH    "ins_test.ass"
#define INSTRUCTION_LEN     12

#define MEMORY_SIZE         4096
#define INS_START_POS        0

#define REGISTER_NUM        32

// MASKes
#define OP_MASK             0xfc000000
#define RS_MASK             0x03e00000
#define RT_MASK             0x001f0000
#define RD_MASK             0x0000f800
#define SH_MASK             0x000007c0
#define FN_MASK             0x0000003f
#define IM_MASK             0x0000ffff

// SHIFT
//#define OP_SHF              26
#define RS_SHF              21
#define RT_SHF              16
#define RD_SHF              11
#define SHM_SHF             6

//Instruction Op
// I format -- OPcode
#define I_ADDI              0x08
#define I_ADDIU             0x09
#define I_SLTI              0x0a
//#define I_SLTIU
//#define I_SLTIU

// R format -- SHM & FUNC




typedef enum {FORMAT_I, FORMAT_R} op_format;
typedef enum {REG_RS, REG_RT, REG_RD} reg_type;


// pipeline register types

typedef struct {
  unsigned int ins;
  unsigned int flush;
  unsigned int nextPC;
  // maybe more
} IFID_Register;

typedef struct {
  unsigned int wb;
  unsigned int mem;
  unsigned int ex;
  unsigned int reg1Value;
  unsigned int reg2Value;
  unsigned int extendValue;
  unsigned short rs;
  unsigned short rt;
  unsigned short rd;
} IDEX_Register;

typedef struct {
  unsigned int wb;
  unsigned int m;
  unsigned int aluResult;
  unsigned int writeData;
  unsigned short rd;
} EXMEM_Register;

typedef struct {
  un
}

typedef struct {
  // not sure yet
  unsigned int mem;
  unsigned int memValue;
  unsigned int aluValue;
  unsigned int rd;
} shadow_registers;


#endif
