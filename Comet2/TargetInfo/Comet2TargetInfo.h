//===-- Comet2TargetInfo.h - COMET-II Target Implementation -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_TARGETINFO_COMET2TARGETINFO_H
#define LLVM_LIB_TARGET_COMET2_TARGETINFO_COMET2TARGETINFO_H

namespace llvm {

class Target;

Target &getTheComet2Target();

} // namespace llvm

#endif // LLVM_LIB_TARGET_COMET2_TARGETINFO_COMET2TARGETINFO_H
