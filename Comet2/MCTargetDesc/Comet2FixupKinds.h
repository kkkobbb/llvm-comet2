//===-- Comet2FixupKinds.h - COMET-II Specific Fixup Entries ----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2FIXUPKINDS_H
#define LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2FIXUPKINDS_H

#include "llvm/MC/MCFixup.h"

#undef Comet2

namespace llvm {
namespace Comet2 {
enum Fixups {
  fixup_comet2_24 = FirstTargetFixupKind,  // TODO 定義はこれでいい？
  // fixup_comet2_invalid - used as a sentinel and a marker, must be last fixup
  fixup_comet2_invalid,
  NumTargetFixupKinds = fixup_comet2_invalid - FirstTargetFixupKind
};
} // end namespace Comet2
} // end namespace llvm

#endif
