
#include "llvm/Transforms/Obfuscation/BogusControlFlow.h"
#include "llvm/Transforms/Obfuscation/Flattening.h"
#include "llvm/Transforms/Obfuscation/Split.h"
#include "llvm/Transforms/Obfuscation/Substitution.h"
#include "llvm/CryptoUtils.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "kllvm/Transforms/Obfuscation/KStringEncode.h"
#include "llvm/Transforms/Utils.h"
using namespace llvm;

static cl::opt<bool> Flattening("fla", cl::init(false),
                                cl::desc("Enable the flattening pass"));

static cl::opt<bool> BogusControlFlow("bcf", cl::init(false),
                                      cl::desc("Enable bogus control flow"));

static cl::opt<bool> Substitution("sub", cl::init(false),
                                  cl::desc("Enable instruction substitutions"));

static cl::opt<std::string> AesSeed("aesSeed", cl::init(""),
                                    cl::desc("seed for the AES-CTR PRNG"));

static cl::opt<bool> Split("split", cl::init(false),
                           cl::desc("Enable basic block splitting"));

static cl::opt<bool> KString("kstr", cl::init(false),
                           cl::desc("Enable string encode"));

static llvm::RegisterStandardPasses Y(
        llvm::PassManagerBuilder::EP_EarlyAsPossible,
        [](const llvm::PassManagerBuilder &Builder,
           llvm::legacy::PassManagerBase &PM) {
            if(!AesSeed.empty()) {
                if(!llvm::cryptoutils->prng_seed(AesSeed.c_str()))
                    exit(1);
            }

            if(Flattening){
                PM.add(createLowerSwitchPass());
            }
            PM.add(createSplitBasicBlock(Split));
            PM.add(createBogus(BogusControlFlow));
            PM.add(createFlattening(Flattening));
            PM.add(createSubstitution(Substitution));
            PM.add(createKStringEncode(KString));
        });