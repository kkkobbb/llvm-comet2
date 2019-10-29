//===-- Comet2MCTargetDesc.h - COMET-II Target Descriptions -----*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// This file provides COMET-II specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2MCTARGETDESC_H
#define LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2MCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCTargetOptions.h"
#include "llvm/Support/DataTypes.h"
#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class StringRef;
class Target;
class Triple;
class raw_ostream;
class raw_pwrite_stream;

MCCodeEmitter *createComet2MCCodeEmitter(const MCInstrInfo &MCII,
                                         const MCRegisterInfo &MRI,
                                         MCContext &Ctx);

MCAsmBackend *createComet2AsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                     const MCRegisterInfo &MRI,
                                     const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter> createComet2ELFObjectWriter(uint8_t OSABI,
                                                                  bool Is64Bit);
}

// Defines symbolic names for COMET-II registers.
#define GET_REGINFO_ENUM
#include "Comet2GenRegisterInfo.inc"

// Defines symbolic names for COMET-II instructions.
#define GET_INSTRINFO_ENUM
#include "Comet2GenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "Comet2GenSubtargetInfo.inc"

#endif
