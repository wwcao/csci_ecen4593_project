#include "static.h"
#include "units.h"
#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

#define IS_PIPELINE             1
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
bool init_ins;

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

unsigned int forwardA;		//rs fwding mux
unsigned int forwardB;		//rt fwding mux
unsigned int IS_FWDING;		// check fwding happen?

unsigned int writedata;
bool Stall;

unsigned int run_pipeline;

void IF(void);
void ID(void);
void EX(void);
void MEM(void);
void WB(void);
void start(void);

void aluUnitOperation(unsigned int src1, unsigned int src2);
// controll

void ctlUnitOperation(unsigned int opCode,
				unsigned int regVal1, unsigned int regVal2,
				unsigned int extendedValue);
void hdUnitOperation(void);
void fwdUnitOperation(unsigned int *src1, unsigned int *src2);
void wirtetoPipelineRegs(void);

void init_pipeline();

#endif
