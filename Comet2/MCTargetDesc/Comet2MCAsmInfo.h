//===-- Comet2MCAsmInfo.h - COMET-II Asm Info ------------------*- C++ -*--===//
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

// NOTE llvm/include/llvm/MC/MCAsmInfo.h
// NOTE llvm/lib/MC/MCAsmInfo.cpp
class Comet2MCAsmInfo : public MCAsmInfoELF {
  void anchor() override;

public:
  explicit Comet2MCAsmInfo(const Triple &TargetTriple);
};

} // namespace llvm

#endif
