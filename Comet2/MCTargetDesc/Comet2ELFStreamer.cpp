//===-- Comet2ELFStreamer.cpp - COMET-II ELF Target Streamer Methods ------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides COMET-II specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "Comet2ELFStreamer.h"
#include "MCTargetDesc/Comet2AsmBackend.h"
#include "Comet2MCTargetDesc.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCSubtargetInfo.h"

using namespace llvm;

// This part is for ELF object output.
Comet2TargetELFStreamer::Comet2TargetELFStreamer(MCStreamer &S,
                                                 const MCSubtargetInfo &STI)
    : Comet2TargetStreamer(S) {
  MCAssembler &MCA = getStreamer().getAssembler();

  unsigned EFlags = MCA.getELFHeaderEFlags();

  MCA.setELFHeaderEFlags(EFlags);
}

MCELFStreamer &Comet2TargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
