#include "llvm/Transforms/Runahead/InsertJRE.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/InlineAsm.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Type.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"

using namespace llvm;

PreservedAnalyses InsertRunaheadTriggerPass::run(Loop &L, LoopAnalysisManager &LAM,
                      LoopStandardAnalysisResults &LSAR, LPMUpdater &LU) {

                        
  // 1. Get the Preheader
  BasicBlock *Preheader = L.getLoopPreheader();
  if (!Preheader) {
    // This loop doesn't have a preheader (might not be in canonical form).
    // You might want to run LoopSimplify pass first, or just skip.
    errs() << "Skipping loop without preheader in function "
           << L.getHeader()->getParent()->getName() << "\n";
    return PreservedAnalyses::all();
  }

  // 2. Get the Exit Block(s)
  SmallVector<BasicBlock *, 4> ExitBlocks;
  L.getExitBlocks(ExitBlocks);

  // --- Decision Point: How to handle multiple exits? ---
  // Instruction `jre $0` seems to take ONE target address.
  // Simplest Approach: Only handle loops with a single exit block.
  if (ExitBlocks.size() != 1) {
    errs() << "Skipping loop with " << ExitBlocks.size()
           << " exit blocks in function "
           << L.getHeader()->getParent()->getName() << "\n";
    // Optionally, implement a more complex strategy here:
    // - Pick a specific exit (e.g., based on profile data)?
    // - Target a common post-dominator?
    // - Insert multiple triggers (if hardware supports it)?
    return PreservedAnalyses::all();
  }
  BasicBlock *ExitBlock = ExitBlocks[0];

  // 3. Get Insertion Point in Preheader
  // Insert *before* the terminator instruction of the preheader.
  Instruction *InsertPt = Preheader->getTerminator();
  if (!InsertPt) {
    // Should not happen in well-formed IR
    errs() << "Preheader has no terminator instruction!\n";
    return PreservedAnalyses::all();
  }

  // 4. Construct the Inline Assembly Instruction
  LLVMContext &Ctx = Preheader->getContext();
  Function *F = Preheader->getParent();

  // Get the block address of the exit block
  BlockAddress *ExitBlockAddr = BlockAddress::get(F, ExitBlock);

  // Define the type of the inline assembly function: void (i8*)
  // The 's' constraint takes a symbolic address, often passed as i8*.
  //   Type *ArgType = Type::getInt8PtrTy(Ctx); // Type matching the 's' constraint
  Type *ArgType = PointerType::get(Ctx, 0); // Type matching the 's' constraint
  FunctionType *AsmFuncType =
      FunctionType::get(Type::getVoidTy(Ctx), {ArgType}, false);

  // Create the InlineAsm value
  // Adjust dialect (AD_ATT or AD_Intel) if necessary, though likely not
  // critical for this.
  InlineAsm *IA =
      InlineAsm::get(AsmFuncType,
                     "jre $0", // Your assembly string
                     "s",      // Constraint: 's' for symbolic address
                     /*hasSideEffects=*/true,
                     /*isAlignStack=*/false, // Usually false for non-calls
                     InlineAsm::AD_ATT,      // Or AD_Intel
                     /*canThrow=*/false);

  // 5. Create the Call Instruction
  CallInst::Create(AsmFuncType, IA, {ExitBlockAddr}, "", InsertPt);

  LLVM_DEBUG(dbgs() << "Inserted runahead trigger in preheader of loop in "
                    << F->getName() << ", targeting exit block "
                    << ExitBlock->getName() << "\n");

  // We modified the IR, so analyses are no longer preserved.
  return PreservedAnalyses::none();
}
