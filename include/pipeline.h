#include "static.h"
#ifndef __PIPELINE_HEADER__
#define __PIPELINE_HEADER__

#define IS_PIPELINE         0
#define RUNTOEND            0

void IF() {

}

void ID() {

}

void EX() {

}

void MEM() {

}

void WB() {

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
