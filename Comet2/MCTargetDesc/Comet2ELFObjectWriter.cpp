//===-- Comet2ELFObjectWriter.cpp - COMET-II ELF Writer -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/Comet2FixupKinds.h"
#include "MCTargetDesc/Comet2MCExpr.h"
#include "MCTargetDesc/Comet2MCTargetDesc.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

// NOTE llvm/include/llvm/BinaryFormat/ELF.h
// NOTE llvm/include/llvm/BinaryFormat/ELFRelocs/*.def
// NOTE llvm/lib/MC/MCELFObjectTargetWriter.cpp

namespace {
class Comet2ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  Comet2ELFObjectWriter(uint8_t OSABI, bool Is64Bit);

  ~Comet2ELFObjectWriter() override;

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
};
}

Comet2ELFObjectWriter::Comet2ELFObjectWriter(uint8_t OSABI, bool Is64Bit)
    : MCELFObjectTargetWriter(Is64Bit, OSABI, ELF::EM_NONE,
                              /*HasRelocationAddend*/ true) {}

Comet2ELFObjectWriter::~Comet2ELFObjectWriter() {}

unsigned Comet2ELFObjectWriter::getRelocType(MCContext &Ctx,
                                            const MCValue &Target,
                                            const MCFixup &Fixup,
                                            bool IsPCRel) const {
  // Determine the type of the relocation
  unsigned Kind = Fixup.getKind();

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case Comet2::fixup_comet2_24:
    return ELF::R_MIPS_26;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createComet2ELFObjectWriter(uint8_t OSABI, bool Is64Bit) {
  return llvm::make_unique<Comet2ELFObjectWriter>(OSABI, Is64Bit);
}
