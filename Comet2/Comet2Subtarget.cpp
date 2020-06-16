//===-- Comet2Subtarget.cpp - COMET-II Subtarget Information --------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
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

#define DEBUG_TYPE "comet2-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "Comet2GenSubtargetInfo.inc"

void Comet2Subtarget::anchor() {}

Comet2Subtarget::Comet2Subtarget(const Triple &TT, StringRef CPU, StringRef FS,
                               StringRef ABIName, const TargetMachine &TM)
    : Comet2GenSubtargetInfo(TT, CPU, FS),
      FrameLowering(*this),
      InstrInfo(), RegInfo(getHwMode()), TLInfo(TM, *this) {
  std::string CPUName = "comet2";

  ParseSubtargetFeatures(CPUName, FS);
}

