//===-- Comet2TargetMachine.cpp - Define TargetMachine for COMET-II -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about COMET-II target spec.
//
//===----------------------------------------------------------------------===//

#include "Comet2TargetMachine.h"
#include "Comet2.h"
#include "TargetInfo/Comet2TargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/GlobalISel/IRTranslator.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelect.h"
#include "llvm/CodeGen/GlobalISel/Legalizer.h"
#include "llvm/CodeGen/GlobalISel/RegBankSelect.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Support/FormattedStream.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Target/TargetOptions.h"
using namespace llvm;

extern "C" void LLVMInitializeComet2Target() {
  RegisterTargetMachine<Comet2TargetMachine> X(getTheComet2Target());
}

static StringRef computeDataLayout(const Triple &TT) {
  if (TT.isArch64Bit()) {
    return "e-m:e-p:64:64-i64:64-i128:128-n64-S128";
  } else {
    assert(TT.isArch32Bit() && "only RV32 and RV64 are currently supported");
    return "e-m:e-p:32:32-i64:64-n32-S128";
  }
}

static Reloc::Model getEffectiveRelocModel(const Triple &TT,
                                           Optional<Reloc::Model> RM) {
  if (!RM.hasValue())
    return Reloc::Static;
  return *RM;
}

Comet2TargetMachine::Comet2TargetMachine(const Target &T, const Triple &TT,
                                       StringRef CPU, StringRef FS,
                                       const TargetOptions &Options,
                                       Optional<Reloc::Model> RM,
                                       Optional<CodeModel::Model> CM,
                                       CodeGenOpt::Level OL, bool JIT)
    : LLVMTargetMachine(T, computeDataLayout(TT), TT, CPU, FS, Options,
                        getEffectiveRelocModel(TT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<Comet2ELFTargetObjectFile>()),
      Subtarget(TT, CPU, FS, Options.MCOptions.getABIName(), *this) {
  initAsmInfo();
}

TargetTransformInfo
Comet2TargetMachine::getTargetTransformInfo(const Function &F) {
  return TargetTransformInfo(Comet2TTIImpl(this, F));
}

namespace {
class Comet2PassConfig : public TargetPassConfig {
public:
  Comet2PassConfig(Comet2TargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  Comet2TargetMachine &getComet2TargetMachine() const {
    return getTM<Comet2TargetMachine>();
  }

  bool addInstSelector() override;
};
}

TargetPassConfig *Comet2TargetMachine::createPassConfig(PassManagerBase &PM) {
  return new Comet2PassConfig(*this, PM);
}

bool Comet2PassConfig::addInstSelector() {
  addPass(createComet2ISelDag(getComet2TargetMachine()));

  return false;
}
