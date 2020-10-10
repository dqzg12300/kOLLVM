//
// Created by king on 2020/10/7.
//

#ifndef OLLVM_KSTRINGENCODE_H
#define OLLVM_KSTRINGENCODE_H

#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/ADT/Statistic.h"
#include "llvm/Transforms/Utils/Local.h" // For DemoteRegToStack and DemotePHIToStack
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/CryptoUtils.h"
#include "llvm/Transforms/Obfuscation/Utils.h"

using namespace std;

namespace llvm {
	Pass *createKStringEncode();
	Pass *createKStringEncode(bool flag);
}

#endif //OLLVM_KSTRINGENCODE_H
