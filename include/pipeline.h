#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

#include "static.h"
#include "units.h"
#include "utils.h"
#include "cache.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define IS_PIPELINE             1       // DON'T set to 0 (test assembly not compatible)

unsigned int pcSrc1;
unsigned int pcSrc2;
bool PCSrc;

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

int writedata;
bool Stall_harzard;
bool Stall_cachemissed;

void IF(void);
void ID(void);
void EX(void);
void MEM(void);
void WB(void);
void startPipeline(void);

void aluUnitOperation(int src1, int src2);
void ctlUnitOperation(unsigned int opCode,
				int regVal1, int regVal2,
				int extendedValue);
void hdUnitOperation(void);
void fwdUnitEX(int *src1, int *src2);
void fwdUnitID(unsigned int rs, unsigned int rt, int *src1, int *src2);
void transferPipelineRegs(void);

void init_pipeline();
void init_pipeline_regs();
void init_wireOutputs();
void insertNOP();

#endif
