//===-- Comet2MCAsmInfo.cpp - COMET-II Asm properties ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declarations of the Comet2MCAsmInfo properties.
//
//===----------------------------------------------------------------------===//

#include "Comet2MCAsmInfo.h"
#include "MCTargetDesc/Comet2MCExpr.h"
#include "llvm/ADT/Triple.h"
#include "llvm/BinaryFormat/Dwarf.h"
#include "llvm/MC/MCStreamer.h"
using namespace llvm;

void Comet2MCAsmInfo::anchor() {}

Comet2MCAsmInfo::Comet2MCAsmInfo(const Triple &TT) {
  CodePointerSize = CalleeSaveStackSlotSize = 2;
  CommentString = ";";
  LabelSuffix = " ";
  AlignmentIsInBytes = false;
  UsesELFSectionDirectiveForBSS = true;
  UseIntegratedAssembler = false;
  SupportsDebugInformation = false;
  HasDotTypeDotSizeDirective = false;
  HasFunctionAlignment = false;
  HasSingleParameterDotFile = false;
  Data16bitsDirective = "\t.word\t";
}

bool
Comet2MCAsmInfo::shouldOmitSectionDirective(StringRef SectionName) const {
    return true;
}
