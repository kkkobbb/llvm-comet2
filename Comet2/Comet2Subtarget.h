//===-- Comet2Subtarget.h - Define Subtarget for the COMET-II ---*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// This file declares the COMET-II specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2SUBTARGET_H
#define LLVM_LIB_TARGET_COMET2_COMET2SUBTARGET_H

#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "Comet2GenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class Comet2Subtarget : public Comet2GenSubtargetInfo {
  virtual void anchor() {};
  bool ExtendedInsts;

public:
  // Initializes the data members to match that of the specified triple.
  Comet2Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                 StringRef ABIName, const TargetMachine &TM);

  // Parses features string setting specified subtarget options. The
  // definition of this function is auto-generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef FS);
};
} // End llvm namespace

#endif