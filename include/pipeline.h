#include "static.h"
#include "units.h"
#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

#define IS_PIPELINE             0
#define RUNTOEND                0

unsigned int memory[MEMORY_SIZE];
unsigned int register_file[REGISTER_NUM];
unsigned int PC;

unsigned int pcSrc1;
unsigned int pcSrc2;
bool PCSrc;
//bool PCdisable;
bool Flush_if;
bool Flush_id;
bool Flush_ex;

// regular pipeline register
IFID_Register ifid_reg;
IDEX_Register idex_reg;
EXMEM_Register exmem_reg;
MEMWB_Register memwb_reg;

// wired output storage == shadow pipeline register
IFID_Register _ifid_reg;
IDEX_Register _idex_reg;
EXMEM_Register _exmem_reg;
MEMWB_Register _memwb_reg;

stage cStage;			// curent state for non_pipeline
stage nStage;			// next state

void IF(void);
void ID(void);
void EX(void);
void MEM(void);
void WB(void);
void start(void);

void aluUnitOperation(void);
// controll

void ctlUnitOperation(unsigned int opCode,
				unsigned int regVal1, unsigned int regVal2,
				unsigned int extendedValue);
void hdUnitOperation(void);
void fwdUnitOperation(void);

#endif
