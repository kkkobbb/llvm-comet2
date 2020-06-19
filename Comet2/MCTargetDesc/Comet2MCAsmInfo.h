//===-- Comet2MCAsmInfo.h - COMET-II Asm Info ------------------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the Comet2MCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2MCASMINFO_H
#define LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2MCASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

// NOTE llvm/include/llvm/MC/MCAsmInfo.h MCAsmInfo
// NOTE llvm/lib/MC/MCAsmInfo.cpp MCAsmInfo
class Comet2MCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit Comet2MCAsmInfo(const Triple &TargetTriple);
  bool shouldOmitSectionDirective(StringRef SectionName) const override;
};

} // namespace llvm

#endif
