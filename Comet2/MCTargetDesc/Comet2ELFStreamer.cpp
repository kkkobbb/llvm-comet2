//===-- Comet2ELFStreamer.cpp - COMET-II ELF Target Streamer Methods ------===//
//
//===----------------------------------------------------------------------===//
//
// This file provides COMET-II specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "Comet2ELFStreamer.h"
#include "MCTargetDesc/Comet2AsmBackend.h"
#include "Comet2MCTargetDesc.h"
#include "Utils/Comet2BaseInfo.h"
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

MCELFStreamer &RISCVTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
