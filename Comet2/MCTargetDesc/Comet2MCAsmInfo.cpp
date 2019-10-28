//===-- Comet2MCAsmInfo.cpp - COMET-II Asm properties ---------------------===//
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
  CommentString = "#";
  AlignmentIsInBytes = false;
  //SupportsDebugInformation = true;
  //ExceptionsType = ExceptionHandling::DwarfCFI;
  Data16bitsDirective = "\t.word\t";
}
