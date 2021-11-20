//===-- Comet2Subtarget.h - Define Subtarget for the COMET-II ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the COMET-II specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2SUBTARGET_H
#define LLVM_LIB_TARGET_COMET2_COMET2SUBTARGET_H

#include "Comet2FrameLowering.h"
#include "Comet2InstrInfo.h"
#include "Comet2ISelLowering.h"
#include "llvm/CodeGen/TargetSubtargetInfo.h"
#include "llvm/Target/TargetMachine.h"

#define GET_SUBTARGETINFO_HEADER
#include "Comet2GenSubtargetInfo.inc"

namespace llvm {
class StringRef;

class Comet2Subtarget : public Comet2GenSubtargetInfo {
  virtual void anchor();
  bool ExtendedInsts;
  Comet2FrameLowering FrameLowering;
  Comet2InstrInfo InstrInfo;
  Comet2RegisterInfo RegInfo;
  Comet2TargetLowering TLInfo;

public:
  // Initializes the data members to match that of the specified triple.
  Comet2Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                  StringRef ABIName, const TargetMachine &TM);

  // Parses features string setting specified subtarget options. The
  // definition of this function is auto-generated by tblgen.
  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);

  const Comet2FrameLowering *getFrameLowering() const override {
    return &FrameLowering;
  }
  const Comet2InstrInfo *getInstrInfo() const override { return &InstrInfo; }
  const Comet2RegisterInfo *getRegisterInfo() const override {
    return &RegInfo;
  }
  const Comet2TargetLowering *getTargetLowering() const override {
    return &TLInfo;
  }
};
} // End llvm namespace

#endif
