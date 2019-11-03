//===-- Comet2ELFObjectWriter.cpp - COMET-II ELF Writer -------------------===//
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

namespace {
class Comet2ELFObjectWriter : public MCELFObjectTargetWriter {
public:
  Comet2ELFObjectWriter(uint8_t OSABI, bool Is64Bit);

  ~Comet2ELFObjectWriter() override;

  // Return true if the given relocation must be with a symbol rather than
  // section plus offset.
  bool needsRelocateWithSymbol(const MCSymbol &Sym,
                               unsigned Type) const override {
    // TODO: this is very conservative, update once RISC-V psABI requirements
    //       are clarified.
    return true;
  }

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
  unsigned Kind = Fixup.getTargetKind();

  switch (Kind) {
  default:
    llvm_unreachable("invalid fixup kind!");
  case Comet2::fixup_comet2_24:
    return ELF::R_MIPS_26;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createComet2ELFObjectWriter(uint8_t OSABI, bool Is64Bit) {
  return std::make_unique<Comet2ELFObjectWriter>(OSABI, Is64Bit);
}
