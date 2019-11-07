//===-- Comet2TargetInfo.cpp - COMET-II Target Implementation -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/Comet2TargetInfo.h"
#include "llvm/Support/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheComet2Target() {
  static Target TheComet2Target;
  return TheComet2Target;
}

extern "C" void LLVMInitializeComet2TargetInfo() {
  RegisterTarget<Triple::comet2> X(getTheComet2Target(), "comet2",
                                    "COMET-II", "Comet2");
}
