//===-- Comet2TargetObjectFile.h - COMET-II Object Info -*- C++ ---------*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2TARGETOBJECTFILE_H
#define LLVM_LIB_TARGET_COMET2_COMET2TARGETOBJECTFILE_H

#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"

namespace llvm {
class Comet2TargetMachine;

/// This implementation is used for Comet2 ELF targets.
class Comet2ELFTargetObjectFile : public TargetLoweringObjectFileELF {

public:
  void Initialize(MCContext &Ctx, const TargetMachine &TM) override;
};

} // end namespace llvm

#endif
