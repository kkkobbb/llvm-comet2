//===-- Comet2TargetStreamer.cpp - COMET-II Target Streamer Methods -------===//
//
//===----------------------------------------------------------------------===//
//
// This file provides COMET-II specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "Comet2TargetStreamer.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

Comet2TargetStreamer::Comet2TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

// This part is for ascii assembly output
Comet2TargetAsmStreamer::Comet2TargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : Comet2TargetStreamer(S), OS(OS) {}
