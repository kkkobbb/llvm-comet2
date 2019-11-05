//===-- Comet2ELFStreamer.h - COMET-II ELF Target Streamer -----*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2ELFSTREAMER_H
#define LLVM_LIB_TARGET_COMET2_COMET2ELFSTREAMER_H

#include "Comet2TargetStreamer.h"
#include "llvm/MC/MCELFStreamer.h"

namespace llvm {

class Comet2TargetELFStreamer : public Comet2TargetStreamer {
public:
  MCELFStreamer &getStreamer();
  Comet2TargetELFStreamer(MCStreamer &S, const MCSubtargetInfo &STI);
};
}
#endif
