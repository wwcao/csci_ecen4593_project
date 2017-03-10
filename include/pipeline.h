#include "static.h"
#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

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

void start_pipeline(void) {
  WB();
  MEM();
  EX();
  ID();
  IF();
}

void start(void) {
  IF();
  ID();
  EX();
  MEM();
  WB();
}

#endif
