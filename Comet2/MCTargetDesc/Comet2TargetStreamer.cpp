//===-- Comet2TargetStreamer.cpp - COMET-II Target Streamer Methods -------===//
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

#include "Comet2TargetStreamer.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

Comet2TargetStreamer::Comet2TargetStreamer(MCStreamer &S) : MCTargetStreamer(S) {}

// This part is for ascii assembly output
Comet2TargetAsmStreamer::Comet2TargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : Comet2TargetStreamer(S), OS(OS) {}
