//===-- Comet2ELFStreamer.h - COMET-II ELF Target Streamer -----*- C++ -*--===//
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
