//===-- Comet2Subtarget.cpp - COMET-II Subtarget Information --------------===//
//
//===----------------------------------------------------------------------===//
//
// This file implements the COMET-II specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "Comet2Subtarget.h"
#include "Comet2.h"
#include "llvm/Support/TargetRegistry.h"
#include <string>

using namespace llvm;

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Comet2GenSubtargetInfo.inc"

void Comet2Subtarget::anchor() {}

Comet2Subtarget::Comet2Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               StringRef ABIName, const TargetMachine &TM)
    : Comet2GenSubtargetInfo(TT, CPU, FS),
      FrameLowering(initializeSubtargetDependencies(TT, CPU, FS, ABIName)),
      InstrInfo(), RegInfo(getHwMode()), TLInfo(TM, *this) {
  std::string CPUName = "generic";

  ParseSubtargetFeatures(CPUName, FS);
}

