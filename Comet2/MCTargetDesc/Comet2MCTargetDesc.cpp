//===-- Comet2MCTargetDesc.cpp - COMET-II Target Descriptions -------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// This file provides COMET-II-specific target descriptions.
///
//===----------------------------------------------------------------------===//

#include "Comet2MCTargetDesc.h"
#include "Comet2ELFStreamer.h"
#include "Comet2InstPrinter.h"
#include "Comet2MCAsmInfo.h"
#include "Comet2TargetStreamer.h"
#include "TargetInfo/Comet2TargetInfo.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/CodeGen/Register.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "Comet2GenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "Comet2GenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "Comet2GenSubtargetInfo.inc"

using namespace llvm;

static MCInstrInfo *createComet2MCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitComet2MCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createComet2MCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitComet2MCRegisterInfo(X, Comet2::GR6);
  return X;
}

static MCAsmInfo *createComet2MCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new Comet2MCAsmInfo(TT);
  return MAI;
}

static MCSubtargetInfo *createComet2MCSubtargetInfo(const Triple &TT,
                                                    StringRef CPU, StringRef FS) {
  // TODO subtargetがない場合は？
  std::string CPUName = std::string(CPU);
  return createComet2MCSubtargetInfoImpl(TT, CPUName, FS);
}

static MCInstPrinter *createComet2MCInstPrinter(const Triple &T,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new Comet2InstPrinter(MAI, MII, MRI);
}

static MCTargetStreamer *
createComet2ObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  // TODO RISCVのまま
  const Triple &TT = STI.getTargetTriple();
  if (TT.isOSBinFormatELF())
    return new Comet2TargetELFStreamer(S, STI);
  return nullptr;
}

static MCTargetStreamer *createComet2AsmTargetStreamer(MCStreamer &S,
                                                       formatted_raw_ostream &OS,
                                                       MCInstPrinter *InstPrint,
                                                       bool isVerboseAsm) {
  return new Comet2TargetAsmStreamer(S, OS);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeComet2TargetMC() {
  // NOTE llvm/include/llvm/Support/TargetRegistry.h getTheFooTarget
  Target &T = getTheComet2Target();  // TODO 型は正しい？
  TargetRegistry::RegisterMCAsmInfo(T, createComet2MCAsmInfo);
  TargetRegistry::RegisterMCInstrInfo(T, createComet2MCInstrInfo);
  TargetRegistry::RegisterMCRegInfo(T, createComet2MCRegisterInfo);
  TargetRegistry::RegisterMCAsmBackend(T, createComet2AsmBackend);
  TargetRegistry::RegisterMCCodeEmitter(T, createComet2MCCodeEmitter);
  TargetRegistry::RegisterMCInstPrinter(T, createComet2MCInstPrinter);
  TargetRegistry::RegisterMCSubtargetInfo(T, createComet2MCSubtargetInfo);
  TargetRegistry::RegisterObjectTargetStreamer(
      T, createComet2ObjectTargetStreamer);

  // Register the asm target streamer.
  TargetRegistry::RegisterAsmTargetStreamer(T, createComet2AsmTargetStreamer);
}
