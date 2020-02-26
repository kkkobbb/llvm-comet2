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

// NOTE 参考 llvm/include/llvm/MC/MCAsmInfo.h
Comet2MCAsmInfo::Comet2MCAsmInfo(const Triple &TT) {
  CodePointerSize = CalleeSaveStackSlotSize = 2;
  CommentString = ";";
  // ラベル単体は不可
  LabelSuffix = " NOP ";
  //LabelSuffix = " DS 0 ";
  Data16bitsDirective = "\t.word\t";

  // TODO その他の設定は不要？
  /*
  AlignmentIsInBytes = false;
  UsesELFSectionDirectiveForBSS = true;
  UseIntegratedAssembler = false;
  SupportsDebugInformation = false;
  HasDotTypeDotSizeDirective = false;
  HasFunctionAlignment = false;
  HasSingleParameterDotFile = false;

  GlobalDirective = "; ";

  UsesNonexecutableStackSection = false;
  */
}

bool
Comet2MCAsmInfo::shouldOmitSectionDirective(StringRef SectionName) const {
    return true;
}
