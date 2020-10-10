#ifndef __UTILS_OBF__
#define __UTILS_OBF__

#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/Transforms/Utils/Local.h" // For DemoteRegToStack and DemotePHIToStack
#include <stdio.h>

using namespace llvm;

void fixStack(Function *f);
std::string readAnnotate(Function *f);
bool toObfuscate(bool flag, Function *f, std::string attribute);
void printIns(Instruction* ins);
void printBB(BasicBlock *block);
void printFunction(Function &func);
#endif
