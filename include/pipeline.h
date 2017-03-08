#include "static.h"
#ifndef __PIPELINE_HEADER__
#define __PIPLINE_HEADER__

#define IS_PIPELINE             0
#define RUNTOEND                0

void IF(void) {

}

void ID(void) {

}

void EX(void) {

}

void MEM(void) {

}

void WB(void) {

}

void START() {
  IF(void);
  ID(void);
  EX(void);
  MEM(void);
  WB(void);
}

#endif
