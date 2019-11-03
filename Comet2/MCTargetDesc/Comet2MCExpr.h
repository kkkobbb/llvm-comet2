//===-- Comet2MCExpr.h - COMET-II specific MC expression classes -*- C++ -*-===//
//
//===----------------------------------------------------------------------===//
//
// This file describes COMET-II-specific MCExprs, used for modifiers like
// "%hi" or "%lo" etc.,
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2MCEXPR_H
#define LLVM_LIB_TARGET_COMET2_MCTARGETDESC_COMET2MCEXPR_H

#include "llvm/MC/MCExpr.h"

namespace llvm {

class StringRef;
class MCOperand;
class Comet2MCExpr : public MCTargetExpr {
public:
  enum VariantKind {
    VK_Comet2_None,
    VK_Comet2_CALL,
    VK_Comet2_Invalid
  };

private:
  const MCExpr *Expr;
  const VariantKind Kind;

  explicit Comet2MCExpr(const MCExpr *Expr, VariantKind Kind)
      : Expr(Expr), Kind(Kind) {}

public:
  static const Comet2MCExpr *create(const MCExpr *Expr, VariantKind Kind,
                                   MCContext &Ctx);

  VariantKind getKind() const { return Kind; }

  const MCExpr *getSubExpr() const { return Expr; }

  void printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const override;
  bool evaluateAsRelocatableImpl(MCValue &Res, const MCAsmLayout *Layout,
                                 const MCFixup *Fixup) const override;
  void visitUsedExpr(MCStreamer &Streamer) const override;
  MCFragment *findAssociatedFragment() const override {
    return getSubExpr()->findAssociatedFragment();
  }

  void fixELFSymbolsInTLSFixups(MCAssembler &Asm) const override;

  static bool classof(const MCExpr *E) {
    return E->getKind() == MCExpr::Target;
  }

  static bool classof(const Comet2MCExpr *) { return true; }

  // TODO 不要?
#if 0
  static VariantKind getVariantKindForName(StringRef name);
  static StringRef getVariantKindName(VariantKind Kind);
#endif
};

} // end namespace llvm.

#endif
