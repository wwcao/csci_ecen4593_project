#ifndef __STATIC_HEADER__
#define __STATIC_HEADER__

#define INSTRUCTION_PATH    "instruction.assembly"
#define INSTRUCTION_LEN     12

#define MEMORY_SIZE         4096
#define INS_START_POS        0

#define REGISTER_NUM        32

#define OP_MASK             0xfc000000
#define RS_MASK             0x03e00000
#define RT_MASK             0x001f0000
#define RD_MASK             0x0000f800
#define SH_MASK             0x000007c0
#define FN_MASK             0x0000003f

#define RS_SHF              21
#define RT_SHF              16
#define RD_SHF              11

typedef enum {FORMAT_I, FORMAT_R} op_format;
typedef enum {REG_RS, REG_RT, REG_RD} reg_type;
#endif
