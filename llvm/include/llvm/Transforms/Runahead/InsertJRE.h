#ifndef LLVM_TRANSFORMS_RUNAHEAD_INSERTJRE_H
#define LLVM_TRANSFORMS_RUNAHEAD_INSERTJRE_H

#include "llvm/IR/PassManager.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

namespace llvm {

class InsertRunaheadTriggerPass
    : public PassInfoMixin<InsertRunaheadTriggerPass> {
public:
PreservedAnalyses run(Loop &L, LoopAnalysisManager &LAM,
    LoopStandardAnalysisResults &LSAR, LPMUpdater &LU);
};

} // namespace llvm

#endif // LLVM_TRANSFORMS_RUNAHEAD_INSERTJRE_H
