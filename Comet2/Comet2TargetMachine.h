//===-- Comet2TargetMachine.h - Define TargetMachine for Comet-II -*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// This file declares the COMET-II specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2TARGETMACHINE_H
#define LLVM_LIB_TARGET_COMET2_COMET2TARGETMACHINE_H

#include "Comet2FrameLowering.h"
#include "Comet2InstrInfo.h"
#include "Comet2ISelLowering.h"
#include "Comet2SelectionDAGInfo.h"
#include "Comet2RegisterInfo.h"
#include "Comet2Subtarget.h"
#include "llvm/CodeGen/SelectionDAGTargetInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class Comet2TargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  Comet2Subtarget Subtarget;

public:
  Comet2TargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                     StringRef FS, const TargetOptions &Options,
                     Optional<Reloc::Model> RM, Optional<CodeModel::Model> CM,
                     CodeGenOpt::Level OL, bool JIT);

  const Comet2Subtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  TargetTransformInfo getTargetTransformInfo(const Function &F) override;
};
}

#endif
