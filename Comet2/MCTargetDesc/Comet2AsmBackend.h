//===-- Comet2AsmBackend.h - COMET-II Assembler Backend -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// NOTE llvm/include/llvm/MC/MCAsmBackend.h MCAsmBackend
// NOTE llvm/lib/MC/MCAsmBackend.cpp MCAsmBackend

#ifndef LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2ASMBACKEND_H
#define LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2ASMBACKEND_H

#include "MCTargetDesc/Comet2FixupKinds.h"
#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"

namespace llvm {
class MCAssembler;
class MCObjectTargetWriter;
class raw_ostream;

class Comet2AsmBackend : public MCAsmBackend {
  const MCSubtargetInfo &STI;
  uint8_t OSABI;
  bool Is64Bit;
  const MCTargetOptions &TargetOptions;

public:
  Comet2AsmBackend(const MCSubtargetInfo &STI, uint8_t OSABI, bool Is64Bit,
                  const MCTargetOptions &Options)
      : MCAsmBackend(support::little), STI(STI), OSABI(OSABI), Is64Bit(Is64Bit),
        TargetOptions(Options) {}
  ~Comet2AsmBackend() override {}

  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override;

  bool fixupNeedsRelaxation(const MCFixup &Fixup, uint64_t Value,
                            const MCRelaxableFragment *DF,
                            const MCAsmLayout &Layout) const override {
    return false;
  }

  unsigned getNumFixupKinds() const override {
    return Comet2::NumTargetFixupKinds;
  }

  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override {
    const static MCFixupKindInfo Infos[] = {
      // This table *must* be in the order that the fixup_* kinds are defined in
      // Comet2FixupKinds.h.
      //
      // name                      offset bits  flags
      { "fixup_comet2_24",         0,     24,   0 },  // TODO 定義はこれでいい？
    };
    static_assert((array_lengthof(Infos)) == Comet2::NumTargetFixupKinds,
                  "Not all fixup kinds added to Infos array");

    if (Kind < FirstTargetFixupKind)
      return MCAsmBackend::getFixupKindInfo(Kind);

    assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
           "Invalid kind!");
    return Infos[Kind - FirstTargetFixupKind];
  }

  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }

  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override {
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count) const override;

  const MCTargetOptions &getTargetOptions() const { return TargetOptions; }
};
}

#endif
