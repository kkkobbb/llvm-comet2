//===-- Comet2TargetStreamer.h - COMET-II Target Streamer ------*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_COMET2TARGETSTREAMER_H
#define LLVM_LIB_TARGET_COMET2_COMET2TARGETSTREAMER_H

#include "llvm/MC/MCStreamer.h"

namespace llvm {

class Comet2TargetStreamer : public MCTargetStreamer {
public:
  Comet2TargetStreamer(MCStreamer &S);
};

// This part is for ascii assembly output
class Comet2TargetAsmStreamer : public Comet2TargetStreamer {
  formatted_raw_ostream &OS;

public:
  Comet2TargetAsmStreamer(MCStreamer &S, formatted_raw_ostream &OS);
};

}
#endif
